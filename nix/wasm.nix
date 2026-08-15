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

  # Header-only glm config package for emscripten FIND_ROOT.
  glmPrefix = pkgs.runCommand "glm-headers-wasm" { } ''
    mkdir -p $out/include $out/lib/cmake/glm
    cp -a ${pkgs.glm}/include/. $out/include/
    cat > $out/lib/cmake/glm/glmConfig.cmake <<'EOF_GLM'
set(_glm_inc "''${CMAKE_CURRENT_LIST_DIR}/../../../include")
if(NOT TARGET glm::glm)
  add_library(glm::glm INTERFACE IMPORTED)
  set_target_properties(glm::glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "''${_glm_inc}")
endif()
set(glm_FOUND TRUE)
EOF_GLM
  '';

  # libsigc++ 2.x static for wasm (sigc++-2.0.pc). Meson needs an explicit
  # cross file because it rejects emcc as a "native" compiler.
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

  # Static zlib for wasm (offline; avoids Emscripten -sUSE_ZLIB network port).
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

  # Static SDL2 for wasm32-emscripten.
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

  # Combined prefix: SDL2 + zlib + glm headers + sigc++ for CMAKE_PREFIX_PATH.
  sdlWasmLibs = if sdl2WasmLibs == null then null else pkgs.symlinkJoin {
    name = "construo-wasm-deps";
    paths = [ sdl2WasmLibs zlibWasmLibs glmPrefix sigcWasm ];
  };

  # Full application derivation (requires sdlSrc).
  construo-wasm = if sdlSrc == null then null else pkgs.stdenv.mkDerivation {
    pname = "construo-wasm";
    version = "0.2.3-dev"; # overridden by flake with PROJECT_VERSION_FULL
    src = ../.;
    dontConfigure = true;
    dontUseCmakeConfigure = true;
    nativeBuildInputs = [ pkgs.emscripten pkgs.cmake pkgs.python3 pkgs.pkg-config ];
    env = {
      APP_NAME = "construo";
      SRC_DIR = ".";
      SDL_WASM_LIBS = "${sdlWasmLibs}";
      EXAMPLES_DIR = "./examples";
      PROJECT_VERSION_FULL = "0.2.3-dev";
      SOURCE_URL = "https://github.com/construo-game/construo";
      PKG_CONFIG_PATH = "${sdlWasmLibs}/lib/pkgconfig:${sigcWasm}/lib/pkgconfig";
    };
    buildPhase = ''
      runHook preBuild
      export EM_CACHE="''${TMPDIR:-/tmp}/emcache"
      mkdir -p "$EM_CACHE"
      export PKG_CONFIG_PATH="${sdlWasmLibs}/lib/pkgconfig:${sigcWasm}/lib/pkgconfig''${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"
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
      # Catch whatever emscripten left behind.
      if [ -d build-wasm ]; then
        find build-wasm -maxdepth 1 -type f \( -name '*.html' -o -name '*.js' -o -name '*.wasm' -o -name '*.data' \) \
          -exec cp {} $out/ \; || true
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

in {
  inherit glmPrefix sigcWasm zlibWasmLibs sdl2WasmLibs sdlWasmLibs construo-wasm;

  notes = ''
    .#construo-wasm builds when flake input sdl2-src is wired and
    external helper libraries (geomcpp, logmich, priocpp, sexpcpp, fmt)
    are available as static wasm libraries or via CMAKE_PREFIX_PATH.

    Local non-nix path:
      SDL_SRC=… PREFIX=… mk/wasm/scripts/build-sdl2.sh
      ZLIB_SRC=… mk/wasm/scripts/build-zlib.sh
      SDL_WASM_LIBS=$PREFIX mk/wasm/scripts/build-app.sh
      mk/wasm/scripts/serve.sh
  '';
}
