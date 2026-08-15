# WASM / Emscripten packaging for Construo.
# Adapted from pingus/nix/wasm.nix (simplified: no SDL_image / mixer / sound).
#
# Flake usage:
#   packages.construo-wasm = (import ./nix/wasm.nix {
#     inherit pkgs;
#     sdlSrc = sdl2-src;
#   }).construo-wasm;
#
{ pkgs
, sdlSrc ? null
, sdlVersion ? "2.30.3"
}:

let
  lib = pkgs.lib;
  srcRoot = ../.;

  # Header-only glm config package for emscripten FIND_ROOT.
  glmPrefix = pkgs.runCommand "glm-headers-wasm" { } ''
    mkdir -p $out/include $out/lib/cmake/glm
    cp -a ${../external/glm}/glm $out/include/
    cat > $out/lib/cmake/glm/glmConfig.cmake <<'EOF_GLM'
set(_glm_inc "''${CMAKE_CURRENT_LIST_DIR}/../../../include")
if(NOT TARGET glm::glm)
  add_library(glm::glm INTERFACE IMPORTED)
  set_target_properties(glm::glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "''${_glm_inc}")
endif()
set(glm_FOUND TRUE)
EOF_GLM
  '';

  # libsigc++ 2.x static for wasm (sigc++-2.0.pc).
  emscriptenCrossFile = pkgs.writeText "emscripten-cross.ini" ''
    [binaries]
    c = 'emcc'
    cpp = 'em++'
    ar = 'emar'
    strip = 'emstrip'
    pkg-config = 'pkg-config'

    [host_machine]
    system = 'emscripten'
    cpu_family = 'wasm32'
    cpu = 'wasm32'
    endian = 'little'

    [built-in options]
    default_library = 'static'
  '';

  sigcWasm = pkgs.stdenv.mkDerivation rec {
    pname = "libsigc++-wasm";
    version = "2.12.1";
    src = pkgs.fetchurl {
      url = "mirror://gnome/sources/libsigc++/2.12/libsigc++-${version}.tar.xz";
      hash = "sha256-qdvuMjNR0Qm3ruB0qcuJyj57z4rY7e8YUfTPNZvVCEM=";
    };
    nativeBuildInputs = [ pkgs.emscripten pkgs.meson pkgs.ninja pkgs.pkg-config pkgs.python3 ];
    dontConfigure = true;
    dontUseMesonConfigure = true;
    buildPhase = ''
      runHook preBuild
      export EM_CACHE="''${TMPDIR:-/tmp}/emcache"
      mkdir -p "$EM_CACHE"
      meson setup build \
        --prefix=$out \
        --cross-file=${emscriptenCrossFile} \
        --default-library=static \
        -Dbuild-examples=false \
        -Dbuild-tests=false \
        -Dmaintainer-mode=false
      meson compile -C build
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      meson install -C build
      runHook postInstall
    '';
  };

  zlibWasmLibs = pkgs.stdenv.mkDerivation {
    pname = "zlib-wasm";
    version = pkgs.zlib.version;
    dontUnpack = true;
    dontConfigure = true;
    nativeBuildInputs = [ pkgs.emscripten ];
    env.ZLIB_SRC = "${pkgs.zlib.src}";
    buildPhase = ''
      runHook preBuild
      bash ${../mk/wasm/scripts/build-zlib.sh}
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      cp -a prefix/. $out/
      runHook postInstall
    '';
  };

  sdl2WasmLibs = if sdlSrc == null then null else pkgs.stdenv.mkDerivation {
    pname = "sdl2-wasm";
    version = sdlVersion;
    dontUnpack = true;
    dontConfigure = true;
    dontUseCmakeConfigure = true;
    nativeBuildInputs = [ pkgs.emscripten pkgs.cmake pkgs.python3 ];
    env = {
      SDL_SRC = "${sdlSrc}";
      PREFIX = "prefix";
    };
    buildPhase = ''
      runHook preBuild
      export PREFIX="$PWD/prefix"
      bash ${../mk/wasm/scripts/build-sdl2.sh}
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      if [ -d prefix ]; then
        cp -a prefix/. $out/
      else
        echo "error: SDL2 wasm prefix not produced" >&2
        exit 1
      fi
      runHook postInstall
    '';
  };

  # Generic helper: static CMake lib for wasm from external/<name>.
  mkCmakeWasmLib = { pname, src, extraCmakeArgs ? "", extraPrefix ? null }:
    pkgs.stdenv.mkDerivation {
      inherit pname;
      version = "vendored";
      dontUnpack = true;
      dontConfigure = true;
      dontUseCmakeConfigure = true;
      nativeBuildInputs = [ pkgs.emscripten pkgs.cmake pkgs.python3 pkgs.pkg-config ];
      env = {
        SRC_DIR = "${src}";
        PREFIX = "prefix";
        CMAKE_ARGS = extraCmakeArgs;
        EXTRA_PREFIX = if extraPrefix == null then "" else extraPrefix;
      };
      buildPhase = ''
        runHook preBuild
        export PREFIX="$PWD/prefix"
        export SRC_DIR="${src}"
        export CMAKE_ARGS="${extraCmakeArgs}"
        export EXTRA_PREFIX="${if extraPrefix == null then "" else extraPrefix}"
        bash ${../mk/wasm/scripts/build-cmake-lib.sh}
        runHook postBuild
      '';
      installPhase = ''
        runHook preInstall
        mkdir -p $out
        cp -a prefix/. $out/
        runHook postInstall
      '';
    };

  logmichWasm = mkCmakeWasmLib {
    pname = "logmich-wasm";
    src = ../external/logmich;
    extraCmakeArgs = "-DBUILD_TESTS=OFF";
  };

  sexpcppWasm = mkCmakeWasmLib {
    pname = "sexpcpp-wasm";
    src = ../external/sexpcpp;
    extraCmakeArgs = "-DBUILD_TESTS=OFF -DWARNINGS=OFF";
  };

  # geom is header-only (INTERFACE); install headers + cmake config.
  geomWasm = pkgs.runCommand "geomcpp-wasm" { } ''
    mkdir -p $out/include $out/lib/cmake/geom
    cp -a ${../external/geomcpp}/include/. $out/include/
    cat > $out/lib/cmake/geom/geomConfig.cmake <<'EOF'
set(_geom_inc "''${CMAKE_CURRENT_LIST_DIR}/../../../include")
if(NOT TARGET geom::geom)
  add_library(geom::geom INTERFACE IMPORTED)
  set_target_properties(geom::geom PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "''${_geom_inc}"
    INTERFACE_COMPILE_DEFINITIONS "GLM_ENABLE_EXPERIMENTAL")
endif()
set(geom_FOUND TRUE)
EOF
  '';

  # sexpcpp installs the cmake package "sexp" (sexp::sexp). FIND_ROOT BOTH
  # (see build-cmake-lib.sh) so CMAKE_PREFIX_PATH resolves those prefixes.
  priocppWasm = mkCmakeWasmLib {
    pname = "priocpp-wasm";
    src = ../external/priocpp;
    extraCmakeArgs = "-DBUILD_TESTS=OFF -DBUILD_EXTRA=OFF -DPRIO_USE_JSONCPP=OFF -DPRIO_USE_SEXPCPP=ON -DWARNINGS=OFF -Dsexp_DIR=${sexpcppWasm}/lib/cmake/sexp -Dlogmich_DIR=${logmichWasm}/lib/cmake/logmich";
    extraPrefix = "${logmichWasm}:${sexpcppWasm}";
  };

  # Combined prefix for CMAKE_PREFIX_PATH / PKG_CONFIG_PATH.
  sdlWasmLibs = if sdl2WasmLibs == null then null else pkgs.symlinkJoin {
    name = "construo-wasm-deps";
    paths = [
      sdl2WasmLibs
      zlibWasmLibs
      glmPrefix
      sigcWasm
      logmichWasm
      sexpcppWasm
      geomWasm
      priocppWasm
    ];
  };

  construo-wasm = if sdlSrc == null then null else pkgs.stdenv.mkDerivation {
    pname = "construo-wasm";
    version = "0.2.3-dev";
    src = srcRoot;
    dontConfigure = true;
    dontUseCmakeConfigure = true;
    nativeBuildInputs = [ pkgs.emscripten pkgs.cmake pkgs.python3 pkgs.pkg-config ];
    env = {
      APP_NAME = "construo";
      SRC_DIR = ".";
      SDL_WASM_LIBS = "${sdlWasmLibs}";
      # Resolved to absolute paths inside build-app.sh after unpack.
      PROJECT_VERSION_FULL = "0.2.3-dev";
      SOURCE_URL = "https://github.com/construo-game/construo";
      PKG_CONFIG_PATH = "${sdlWasmLibs}/lib/pkgconfig:${sigcWasm}/lib/pkgconfig";
    };
    buildPhase = ''
      runHook preBuild
      export EM_CACHE="''${TMPDIR:-/tmp}/emcache"
      mkdir -p "$EM_CACHE"
      export PKG_CONFIG_PATH="${sdlWasmLibs}/lib/pkgconfig:${sigcWasm}/lib/pkgconfig''${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
      export CMAKE_PREFIX_PATH="${sdlWasmLibs}"
      bash ${../mk/wasm/scripts/build-app.sh}
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      for f in construo.html construo.js construo.wasm construo.data \
               construo.sdl.html construo.sdl.js construo.sdl.wasm construo.sdl.data; do
        if [ -f "build-wasm/$f" ]; then cp "build-wasm/$f" $out/; fi
        if [ -f "$f" ]; then cp "$f" $out/; fi
      done
      if [ -d build-wasm ]; then
        find build-wasm -maxdepth 1 -type f \( -name '*.html' -o -name '*.js' -o -name '*.wasm' -o -name '*.data' \) \
          -exec cp {} $out/ \; || true
      fi
      # Canonical names for nix run / serve (prefer SUFFIX=.html outputs).
      if [ -f $out/construo.sdl.html ] && [ ! -f $out/construo.html ]; then
        ln -s construo.sdl.html $out/construo.html
      fi
      if [ -f $out/construo.sdl.js ] && [ ! -f $out/construo.js ]; then
        ln -s construo.sdl.js $out/construo.js
      fi
      if [ -f $out/construo.sdl.wasm ] && [ ! -f $out/construo.wasm ]; then
        ln -s construo.sdl.wasm $out/construo.wasm
      fi
      if [ -f $out/construo.sdl.data ] && [ ! -f $out/construo.data ]; then
        ln -s construo.sdl.data $out/construo.data
      fi
      ls -la $out
      runHook postInstall
    '';
    meta = with lib; {
      description = "Construo (WebAssembly / Emscripten)";
      license = licenses.gpl3Plus;
      platforms = platforms.linux;
    };
  };

  mkOpenBrowserApp = {
    pkg
  , appName ? "construo"
  , description ? "Serve Construo wasm in a browser"
  }: {
    type = "app";
    program = toString (pkgs.writeShellScript "serve-${appName}-wasm" ''
      set -euo pipefail
      export PKG=${pkg}
      export APP_NAME=${appName}
      export PATH=${pkgs.python3}/bin:$PATH
      exec bash ${../mk/wasm/scripts/serve.sh} "$@"
    '');
    meta.description = description;
  };

in {
  inherit glmPrefix sigcWasm zlibWasmLibs sdl2WasmLibs sdlWasmLibs;
  inherit logmichWasm sexpcppWasm geomWasm priocppWasm construo-wasm;
  inherit mkOpenBrowserApp;

  notes = ''
    Static wasm helpers (logmich, sexpcpp, geom headers, priocpp) are built
    via mk/wasm/scripts/build-cmake-lib.sh and joined into sdlWasmLibs.

    Local non-nix path:
      SDL_SRC=… PREFIX=… mk/wasm/scripts/build-sdl2.sh
      SRC_DIR=external/logmich PREFIX=… mk/wasm/scripts/build-cmake-lib.sh
      SDL_WASM_LIBS=$PREFIX mk/wasm/scripts/build-app.sh
      mk/wasm/scripts/serve.sh
  '';
}
