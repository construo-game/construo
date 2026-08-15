{
  description = "Masses and springs simulation game";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs?ref=nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";

    # Prebuilt MinGW SDL2 (same source as Pingus).
    SDL2-win32.url = "git+https://github.com/grumnix/SDL2-win32.git";
    SDL2-win32.inputs.nixpkgs.follows = "nixpkgs";

    # SDL2 source tarball for wasm static builds.
    sdl2-src = {
      # 2.30.3 fails on NDK 27: ALooper_pollAll is unavailable (use ALooper_pollOnce).
      url = "https://github.com/libsdl-org/SDL/releases/download/release-2.32.8/SDL2-2.32.8.tar.gz";
      flake = false;
    };
  };

  outputs = { self, nixpkgs, flake-utils, SDL2-win32, sdl2-src }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
        lib = pkgs.lib;

        # Single source of truth: top-level VERSION (e.g. "0.2.3-dev").
        versionBase = lib.strings.removeSuffix "\n" (builtins.readFile ./VERSION);
        gitRev = self.shortRev or self.dirtyShortRev or "unknown";
        revCount = self.revCount or 0;
        construo_version =
          if lib.strings.hasInfix "-dev" versionBase
          then "${versionBase}.${toString revCount}+g${gitRev}"
          else versionBase;

        # tinycmmc helpers (versionFromVERSION / versionFromFile) from vendored tree.
        tinycmmc_lib = import ./external/tinycmmc {
          inherit nixpkgs flake-utils;
        };

        # Stable self for external/* so helper versions do not rebuild on every
        # monorepo commit (same pattern as Pingus).
        selfFor = path: {
          outPath = path;
          shortRev = "vendored";
          dirtyShortRev = "vendored";
          lastModifiedDate = "19700101";
        };

        # Build helper libraries from external/ (squashed subtrees).
        mkLibs = pkgs':
          let
            call = path: args: pkgs'.callPackage path args;
          in
          rec {
            # Header-only glm with platforms = platforms.all (works on MinGW cross).
            glm = call ./external/glm/glm.nix { };

            tinycmmc = call ./external/tinycmmc/tinycmmc.nix {
              self = selfFor ./external/tinycmmc;
              inherit tinycmmc_lib;
            };

            geomcpp = call ./external/geomcpp/geomcpp.nix {
              self = selfFor ./external/geomcpp;
              inherit glm tinycmmc tinycmmc_lib;
              gtest = pkgs'.gtest;
            };

            logmich = call ./external/logmich/logmich.nix { };

            sexpcpp = call ./external/sexpcpp/sexpcpp.nix {
              gtest = pkgs'.gtest;
            };

            priocpp = call ./external/priocpp/priocpp.nix {
              self = selfFor ./external/priocpp;
              inherit logmich;
              sexpcpp = sexpcpp;
              withSexpcpp = true;
              withJsoncpp = true;
              buildTests = false;
              buildExtra = false;
              gtest = pkgs'.gtest;
              jsoncpp = pkgs'.jsoncpp;
              pkg-config = pkgs'.pkg-config;
            };

            xdgcpp =
              if pkgs'.stdenv.hostPlatform.isWindows then null
              else
                pkgs'.stdenv.mkDerivation {
                  pname = "xdgcpp";
                  version = "0.1.0";
                  src = lib.cleanSource ./external/xdgcpp;
                  nativeBuildInputs = [ pkgs'.cmake ];
                };
          };

        libs = mkLibs pkgs;

        commonNative = with pkgs; [ cmake pkg-config ];
        commonLibs = with pkgs; [
          gtest
          zlib
          libsigcxx30
        ] ++ [
          libs.glm
          libs.geomcpp
          libs.logmich
          libs.priocpp
        ] ++ lib.optional (libs.xdgcpp != null) libs.xdgcpp;

        mkConstruo = { pname ? "construo", extraCmakeFlags ? [], extraBuildInputs ? [], postFixup ? "" }:
          pkgs.stdenv.mkDerivation rec {
            inherit pname;
            version = construo_version;
            src = ./.;

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

        # MinGW Win64 game binary (Linux builder).
        win64Libs = mkLibs pkgs.pkgsCross.mingwW64;
        win64Sdl = SDL2-win32.packages.${system}."SDL2-win64";

        construo-win64-bin = pkgs.pkgsCross.mingwW64.stdenv.mkDerivation {
          pname = "construo-win64";
          version = construo_version;
          src = ./.;
          nativeBuildInputs = with pkgs; [ cmake pkg-config ];
          buildInputs = [
            win64Sdl
            win64Libs.glm
            pkgs.pkgsCross.mingwW64.zlib
            pkgs.pkgsCross.mingwW64.libsigcxx
            win64Libs.geomcpp
            win64Libs.logmich
            win64Libs.priocpp
          ];
          cmakeFlags = [
            "-DCONSTRUO_USE_SDL2=ON"
            "-DCONSTRUO_USE_X11=OFF"
            "-DCONSTRUO_USE_GLUT=OFF"
            "-DCONSTRUO_NO_XDGCPP=ON"
            "-DBUILD_TESTS=OFF"
            "-DWARNINGS=ON"
            "-DPROJECT_VERSION_FULL=${construo_version}"
            "-DCMAKE_PREFIX_PATH=${win64Sdl};${win64Libs.glm}"
          ];
          enableParallelBuilding = true;
        };

        # Flat redistributable directory (exe + examples + README).
        construo-win64 = pkgs.runCommand "construo-win64-flat" {
          nativeBuildInputs = [ pkgs.zip ];
        } ''
          mkdir -p $out
          if [ -f ${construo-win64-bin}/bin/construo.sdl.exe ]; then
            cp -a ${construo-win64-bin}/bin/construo.sdl.exe $out/construo.exe
          elif [ -f ${construo-win64-bin}/bin/construo.exe ]; then
            cp -a ${construo-win64-bin}/bin/construo.exe $out/construo.exe
          else
            echo "Win64 binary not produced; packaging scaffolding only." > $out/README.txt
            cp -a ${./examples} $out/examples
            exit 0
          fi
          cp -a ${./examples} $out/examples
          cat > $out/README.txt <<'TXT'
Construo (Win64 SDL2 + GLES2)
Run construo.exe. Example constructions are under examples/.
TXT
        '';

        isWin = pkgs.stdenv.hostPlatform.isWindows;

        # Linux-only: Android + wasm + R36S (same layout as pingus/flake.nix).
        linuxExtras =
          if isWin || !(pkgs.stdenv.hostPlatform.isLinux) then { packages = { }; apps = { }; }
          else
          let
            androidPkgs = import nixpkgs {
              system = pkgs.stdenv.hostPlatform.system;
              config.allowUnfree = true;
              config.android_sdk.accept_license = true;
            };
            buildToolsVersion = "30.0.3";
            packagePlatform = "22";
            compilePlatform = "33";
            ndkVersion = "27.0.12077973";
            targetAbis = [ "armeabi-v7a" "arm64-v8a" ];
            androidSdk = (androidPkgs.androidenv.composeAndroidPackages {
              platformVersions = [ packagePlatform compilePlatform ];
              buildToolsVersions = [ buildToolsVersion ];
              includeNDK = true;
              inherit ndkVersion;
              includeEmulator = false;
              includeSources = false;
            }).androidsdk;

            r36s = import ./nix/r36s.nix {
              inherit (pkgs) lib stdenv stdenvNoCC fetchurl cmake pkg-config writeShellScript zip;
              pkgsCross = pkgs.pkgsCross;
              glm = libs.glm;
            };
            construoR36s = r36s.mkConstruoR36s {
              src = lib.cleanSource ./.;
              version = construo_version;
              pname = "construo-r36s";
            };
            construoR36sPortMaster = r36s.mkConstruoR36sPortMaster {
              r36sPkg = construoR36s;
              version = construo_version;
              pname = "construo-r36s-portmaster";
            };

            android = import ./nix/android.nix {
              pkgs = androidPkgs;
              sdlSrc = sdl2-src;
              sdlVersion = "2.32.8";
              inherit androidSdk buildToolsVersion packagePlatform compilePlatform targetAbis;
            };

            wasm = import ./nix/wasm.nix {
              inherit pkgs;
              sdlSrc = sdl2-src;
              sdlVersion = "2.32.8";
            };

            construoWasm =
              if wasm.construo-wasm != null then
                wasm.construo-wasm.overrideAttrs (old: {
                  __intentionallyOverridingVersion = true;
                  version = construo_version;
                  env = (old.env or { }) // {
                    PROJECT_VERSION_FULL = construo_version;
                  };
                })
              else null;
          construoAndroidPkg = android.mkApk {
            appName = "construo";
            appDir = ./mk/android/app;
            outApkName = "construo.apk";
            keystore = ./mk/android/keystore/debug.keystore;
            gameSrcDir = ./src;
            gameExternalDir = ./external;
            glmIncludeDir = "${libs.glm}/include";
            gameExamplesDir = ./examples;
            gameVersion = construo_version;
          };
          in {
            packages = {
              arkos-sysroot = r36s.arkosSysroot;
              construo-r36s = construoR36s;
              construo-r36s-portmaster = construoR36sPortMaster;
              construo-r36s-portmaster-zip = r36s.mkConstruoR36sPortMasterZip {
                portMasterPkg = construoR36sPortMaster;
                version = construo_version;
                pname = "construo-r36s-portmaster-zip";
              };
              android-sdl-libs = android.sdlAndroidLibs;
              construo-android = construoAndroidPkg;
              wasm-sdl-libs = wasm.sdlWasmLibs;
              wasm-sdl2 = wasm.sdl2WasmLibs;
            } // lib.optionalAttrs (construoWasm != null) {
              construo-wasm = construoWasm;
            };
            apps =
              (lib.optionalAttrs (construoWasm != null) (
                let
                  open = wasm.mkOpenBrowserApp {
                    pkg = construoWasm;
                    appName = "construo";
                    description = "Serve Construo wasm and open in a browser";
                  };
                in {
                  construo-wasm = open;
                  construo-wasm-serve = open;
                }
              ))
              // {
                construo-android = android.mkInstallApp {
                  pkg = construoAndroidPkg;
                  apkFileName = "construo.apk";
                  description = "Install Construo APK via adb";
                };
              };
          };

        packages = rec {
          default = construo;

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


        }
          // lib.optionalAttrs (!isWin) {
            construo-win64 = construo-win64;
            construo-win64-bin = construo-win64-bin;
          }
          // linuxExtras.packages;

        apps =
          {
            default = {
              type = "app";
              program = "${packages.default}/bin/construo";
              meta.description = "Construo (default: X11 binary when available)";
            };
            construo = {
              type = "app";
              program = "${packages.construo}/bin/construo";
              meta.description = "Construo X11/GLUT package";
            };
            construo-sdl = {
              type = "app";
              program = "${packages.construo-sdl}/bin/construo";
              meta.description = "Construo SDL2 + GLES2 package";
            };
          }
          // linuxExtras.apps;

        # Like pingus: every package is a check; verify app program paths.
        appChecks = lib.mapAttrs' (name: app:
          lib.nameValuePair "app-${name}" (
            pkgs.runCommand "check-app-${name}" {
              meta.description = "flake check: apps.${name} program exists";
            } ''
              set -euo pipefail
              if [ ! -e "${app.program}" ]; then
                echo "apps.${name}: program missing: ${app.program}" >&2
                exit 1
              fi
              touch "$out"
            ''
          )
        ) apps;

        version-smoke = pkgs.runCommand "construo-version-smoke" {} ''
          export SDL_VIDEODRIVER=dummy
          export SDL_AUDIODRIVER=dummy
          ${packages.construo-sdl}/bin/construo --version | tee $out
          grep -q "Construo " $out
          grep -q "${construo_version}" $out
        '';

        checks = packages // appChecks // {
          inherit version-smoke;
        };
      in {
        inherit packages apps checks;

        hydraJobs = checks;


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

      }
    );
}
