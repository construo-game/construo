{
  description = "Masses and springs simulation game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    # Still required by logmich / priocpp / geomcpp / sexpcpp flake inputs.
    # Construo itself no longer depends on tinycmmc (CMake helpers are inlined under cmake/).
    tinycmmc.url = "github:grumbel/tinycmmc";
    tinycmmc.inputs.nixpkgs.follows = "nixpkgs";
    tinycmmc.inputs.flake-utils.follows = "flake-utils";

    logmich.url = "github:logmich/logmich";
    logmich.inputs.nixpkgs.follows = "nixpkgs";
    logmich.inputs.tinycmmc.follows = "tinycmmc";

    sexpcpp.url = "github:lispparser/sexp-cpp";
    sexpcpp.inputs.nixpkgs.follows = "nixpkgs";
    sexpcpp.inputs.flake-utils.follows = "flake-utils";
    sexpcpp.inputs.tinycmmc.follows = "tinycmmc";

    priocpp.url = "github:grumbel/priocpp";
    priocpp.inputs.nixpkgs.follows = "nixpkgs";
    priocpp.inputs.flake-utils.follows = "flake-utils";
    priocpp.inputs.sexpcpp.follows = "sexpcpp";
    priocpp.inputs.tinycmmc.follows = "tinycmmc";
    priocpp.inputs.logmich.follows = "logmich";

    geomcpp.url = "github:grumbel/geomcpp";
    geomcpp.inputs.nixpkgs.follows = "nixpkgs";
    geomcpp.inputs.tinycmmc.follows = "tinycmmc";

    xdgcpp.url = "github:grumbel/xdgcpp";
    xdgcpp.inputs.nixpkgs.follows = "nixpkgs";
    xdgcpp.inputs.flake-utils.follows = "flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils, tinycmmc, logmich, sexpcpp, priocpp, geomcpp, xdgcpp }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;
        # Single source of truth: top-level VERSION (e.g. "0.2.3-dev").
        versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
        # shortRev is missing on dirty trees; dirtyShortRev includes "-dirty".
        gitRev = self.shortRev or self.dirtyShortRev or "unknown";
        # revCount is absent for some path/dirty flake evaluations — default to 0.
        revCount = self.revCount or 0;
        # Development builds append .<revCount>+g<shortRev>[-dirty].
        construo_version =
          if lib.strings.hasInfix "-dev" versionBase
          then "${versionBase}.${toString revCount}+g${gitRev}"
          else versionBase;

        commonNative = with pkgs; [ cmake pkg-config ];
        commonLibs = with pkgs; [
          fmt
          glm
          gtest
          zlib
          libsigcxx30
        ] ++ [
          geomcpp.packages.${system}.default
          logmich.packages.${system}.default
          priocpp.packages.${system}.default
          xdgcpp.packages.${system}.default
        ];

        mkConstruo = { pname ? "construo", extraCmakeFlags ? [], extraBuildInputs ? [], postFixup ? "" }:
          pkgs.stdenv.mkDerivation rec {
            inherit pname;
            version = construo_version;
            src = ./.;

            # Pass the fully expanded version into CMake so binaries report the
            # same string as the Nix package (revCount + shortRev).
            cmakeFlags = [
              "-DWARNINGS=ON"
              "-DWERROR=ON"
              "-DBUILD_TESTS=ON"
              "-DPROJECT_VERSION_FULL=${version}"
            ] ++ extraCmakeFlags;

            doCheck = true;
            inherit postFixup;

            nativeBuildInputs = commonNative;
            buildInputs = commonLibs ++ extraBuildInputs;
          };
        # Package set used by packages/checks/hydraJobs outputs.
        packages = rec {
          default = construo;

          # Classic X11 + GLUT Linux build (existing behaviour).
          construo = mkConstruo {
            extraCmakeFlags = [
              "-DCONSTRUO_USE_X11=ON"
              "-DCONSTRUO_USE_GLUT=ON"
              "-DCONSTRUO_USE_SDL2=OFF"
            ];
            extraBuildInputs = with pkgs; [
              freeglut
              libGL
              libGLU
              libX11
            ];
            postFixup = ''
              ln -s $out/bin/construo.x11 $out/bin/construo
            '';
          };

          # Desktop SDL2 + GLES2 validation binary (shared path for ports).
          construo-sdl = mkConstruo {
            pname = "construo-sdl";
            extraCmakeFlags = [
              "-DCONSTRUO_USE_X11=OFF"
              "-DCONSTRUO_USE_GLUT=OFF"
              "-DCONSTRUO_USE_SDL2=ON"
            ];
            extraBuildInputs = with pkgs; [
              SDL2
              libGL
              libglvnd
            ];
            postFixup = ''
              if [ -e $out/bin/construo.sdl ]; then
                ln -s $out/bin/construo.sdl $out/bin/construo
              fi
            '';
          };

          # All native backends in one build (X11 + GLUT + SDL2).
          construo-all = mkConstruo {
            pname = "construo-all";
            extraCmakeFlags = [
              "-DCONSTRUO_USE_X11=ON"
              "-DCONSTRUO_USE_GLUT=ON"
              "-DCONSTRUO_USE_SDL2=ON"
            ];
            extraBuildInputs = with pkgs; [
              freeglut
              libGL
              libGLU
              libX11
              SDL2
              libglvnd
            ];
            postFixup = ''
              if [ -e $out/bin/construo.x11 ]; then
                ln -s $out/bin/construo.x11 $out/bin/construo
              fi
            '';
          };

          # Cross / embedded ports — scaffolding under nix/*.nix and mk/*/.
          # Real outputs need prebuilt SDL2 + static cross deps; until then these
          # stubs fail with a clear message instead of being missing attributes.
          construo-wasm = pkgs.runCommand "construo-wasm-stub" {} ''
            echo "construo-wasm is not packaged yet." >&2
            echo "See nix/wasm.nix, mk/wasm/, and TODO.md (prebuilt SDL2 + static deps)." >&2
            exit 1
          '';
          construo-android = pkgs.runCommand "construo-android-stub" {} ''
            echo "construo-android is not packaged yet." >&2
            echo "See nix/android.nix, mk/android/, and TODO.md." >&2
            exit 1
          '';
          construo-win64 = pkgs.runCommand "construo-win64-stub" {} ''
            echo "construo-win64 is not packaged yet." >&2
            echo "See nix/win32.nix, mk/win32/, and TODO.md (MinGW + SDL2-win32)." >&2
            exit 1
          '';
          construo-r36s = pkgs.runCommand "construo-r36s-stub" {} ''
            echo "construo-r36s is not packaged yet." >&2
            echo "See nix/r36s.nix, mk/r36s/, and TODO.md (ArkOS sysroot)." >&2
            exit 1
          '';
        };

        # Checks attrset (also used by hydraJobs).
        checks = {
          construo = self.packages.${system}.construo;
          construo-sdl = self.packages.${system}.construo-sdl;
          construo-all = self.packages.${system}.construo-all;

          # Ensure --version prints the expanded PROJECT_VERSION_FULL string.
          version-smoke = pkgs.runCommand "construo-version-smoke" {} ''
            export SDL_VIDEODRIVER=dummy
            export SDL_AUDIODRIVER=dummy
            ${self.packages.${system}.construo-sdl}/bin/construo --version | tee $out
            grep -q "Construo " $out
            grep -q "${construo_version}" $out
          '';
        };
      in {
        inherit packages checks;

        # Expose the same set to Hydra / CI consumers.
        hydraJobs = checks // {
          inherit (self.packages.${system}) construo construo-sdl construo-all;
        };

        devShells.default = pkgs.mkShell {
          packages = commonNative ++ commonLibs ++ (with pkgs; [
            freeglut
            libGL
            libGLU
            libX11
            SDL2
            libglvnd
            ninja
            gdb
          ]);
          shellHook = ''
            echo "Construo dev shell — try: cmake --preset linux-all && cmake --build --preset linux-all"
          '';
        };

        apps = {
          default = {
            type = "app";
            program = "${self.packages.${system}.default}/bin/construo";
          };
          construo = {
            type = "app";
            program = "${self.packages.${system}.construo}/bin/construo";
          };
          construo-sdl = {
            type = "app";
            program = "${self.packages.${system}.construo-sdl}/bin/construo";
          };
        };
      }
    );
}
