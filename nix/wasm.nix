# WASM / Emscripten packaging for Construo (scaffolding).
# Pattern follows pingus/nix/wasm.nix; complete once SDL2 wasm prefix and
# static deps (fmt, glm, sigc++, geom, logmich, prio) are available.
#
# Intended flake usage (future):
#   packages.construo-wasm = (import ./nix/wasm.nix { inherit pkgs sdlSrc; }).construo-wasm;
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

in
{
  inherit glmPrefix;

  # Placeholder derivation — enable when sdlSrc + emscripten stdenv wiring is ready.
  # construo-wasm = pkgs.stdenv.mkDerivation { ... emcmake + mk/wasm/scripts/build-app.sh ... };

  notes = ''
    Next steps for a working .#construo-wasm:
    1. Build static SDL2 for wasm32-emscripten (see pingus mk/wasm/scripts/build-sdl2.sh)
    2. Provide fmt, libsigc++-3 (or 2), zlib, geom, logmich, prio as static wasm libs
    3. Invoke mk/wasm/scripts/build-app.sh with SDL_WASM_LIBS and EXAMPLES_DIR
    4. Link with -sFULL_ES2=1 -sALLOW_MEMORY_GROWTH=1 --shell-file mk/wasm/shell.html
  '';
}
