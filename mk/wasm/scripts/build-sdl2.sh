#!/usr/bin/env bash
# Build static SDL2 for wasm32 (adapted from Pingus).
#
# Env:
#   SDL_SRC  - SDL2 source tree (required)
#   PREFIX   - install prefix (default: $PWD/prefix)
#   JOBS     - parallel jobs
set -euo pipefail

export EM_CACHE="${TMPDIR:-/tmp}/emcache"
mkdir -p "$EM_CACHE"

PREFIX="${PREFIX:-$PWD/prefix}"
mkdir -p "$PREFIX/lib" "$PREFIX/include"

if [ -z "${SDL_SRC:-}" ]; then
  echo "error: SDL_SRC required (path to SDL2 sources)" >&2
  exit 1
fi

echo "==> SDL2 static (wasm32) → $PREFIX"
rm -rf SDL2-src build-sdl2
cp -a "$SDL_SRC" SDL2-src
chmod -R u+w SDL2-src
mkdir -p build-sdl2
cd build-sdl2
emcmake cmake ../SDL2-src \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DSDL_SHARED=OFF \
  -DSDL_STATIC=ON \
  -DSDL_TEST=OFF \
  -DSDL_STATIC_PIC=ON
emmake make -j"${NIX_BUILD_CORES:-${JOBS:-$(nproc)}}"
emmake make install
cd ..

if [ ! -f "$PREFIX/lib/libSDL2.a" ]; then
  find build-sdl2 -name 'libSDL2.a' -exec cp {} "$PREFIX/lib/" \; || true
fi
if [ ! -d "$PREFIX/include/SDL2" ] && [ -d SDL2-src/include ]; then
  mkdir -p "$PREFIX/include"
  cp -a SDL2-src/include/. "$PREFIX/include/" || true
fi

echo "==> SDL2 ready under $PREFIX"
ls -la "$PREFIX/lib/libSDL2.a"
ls -la "$PREFIX/include/SDL2/SDL.h" 2>/dev/null || ls -la "$PREFIX/include/SDL.h" 2>/dev/null || true
