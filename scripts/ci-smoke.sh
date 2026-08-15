#!/usr/bin/env bash
# Lightweight CI smoke checks that do not require Nix, EMSDK, NDK, or devices.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "==> Packaging layout checks"
bash scripts/check-port-layouts.sh

echo "==> Critical source files exist"
for f in \
  src/sdl2_display.cpp \
  src/gles2_renderer.cpp \
  src/win32_system.cpp \
  src/print.hpp \
  mk/wasm/shell.html \
  mk/wasm/scripts/build-app.sh \
  mk/android/app/jni/Android.mk \
  mk/r36s/toolchain-arkos-aarch64.cmake \
  flake.nix \
  CMakeLists.txt
 do
  test -e "$f" || { echo "missing $f"; exit 1; }
done
echo "    ok"

echo "==> No leftover git submodules"
test ! -f .gitmodules || { echo "unexpected .gitmodules"; exit 1; }
test -d external/logmich/include
test -d external/geomcpp/include
echo "    ok"

echo "==> CMake defines platform helpers"
grep -q CONSTRUO_TARGET_WINDOWS CMakeLists.txt
grep -q CONSTRUO_TARGET_ANDROID CMakeLists.txt
grep -q CONSTRUO_TARGET_EMSCRIPTEN CMakeLists.txt
grep -q SDL_MAIN_HANDLED CMakeLists.txt
echo "    ok"

echo "ci-smoke passed."
