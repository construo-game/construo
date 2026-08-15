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
        version_file = pkgs.lib.fileContents ./VERSION;
        construo_has_version = ((builtins.substring 0 1) version_file) == "v";
        construo_version = if !construo_has_version
                           then ("0.2.3-${nixpkgs.lib.substring 0 8 self.lastModifiedDate}-${self.shortRev or "dirty"}")
                           else (builtins.substring 1 ((builtins.stringLength version_file) - 2) version_file);

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

            postPatch = ''
              if ${if construo_has_version then "false" else "true"}; then
                echo "${version}" > VERSION
              fi
            '';

            cmakeFlags = [
              "-DWARNINGS=ON"
              "-DWERROR=ON"
              "-DBUILD_TESTS=ON"
            ] ++ extraCmakeFlags;

            doCheck = true;
            inherit postFixup;

            nativeBuildInputs = commonNative;
            buildInputs = commonLibs ++ extraBuildInputs;
          };
      in {
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

        # `nix flake check` builds every currently shippable package.
        # Cross stubs (wasm/android/win64/r36s) are intentionally excluded until
        # prebuilt SDL2 + helper libs exist — including them would fail the check.
        checks = {
          construo = self.packages.${system}.construo;
          construo-sdl = self.packages.${system}.construo-sdl;
          construo-all = self.packages.${system}.construo-all;
        };
      }
    );
}
