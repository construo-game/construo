#!/usr/bin/env bash
# Configure Construo for MinGW cross-compile (x86_64-w64-mingw32).
# Env: MINGW_PREFIX (default /usr/x86_64-w64-mingw32), SDL2 prefix via CMAKE_PREFIX_PATH
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
PREFIX="${MINGW_PREFIX:-/usr/x86_64-w64-mingw32}"
BUILD="${BUILD_DIR:-$ROOT/build-win64}"
mkdir -p "$BUILD"
cd "$BUILD"
cmake "$ROOT" \
  -DCMAKE_SYSTEM_NAME=Windows \
  -DCMAKE_C_COMPILER="${PREFIX}/bin/x86_64-w64-mingw32-gcc" \
  -DCMAKE_CXX_COMPILER="${PREFIX}/bin/x86_64-w64-mingw32-g++" \
  -DCMAKE_FIND_ROOT_PATH="$PREFIX" \
  -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY \
  -DCONSTRUO_USE_SDL2=ON \
  -DCONSTRUO_USE_X11=OFF \
  -DCONSTRUO_USE_GLUT=OFF \
  -DCONSTRUO_NO_XDGCPP=ON \
  -DBUILD_TESTS=OFF \
  "$@"
echo "Configured in $BUILD — run: cmake --build $BUILD"
