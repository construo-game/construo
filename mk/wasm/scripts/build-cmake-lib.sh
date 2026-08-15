#!/usr/bin/env bash
# Build a CMake C++ library as a static wasm32 archive via emcmake.
#
# Env:
#   SRC_DIR      - library source root (required)
#   PREFIX       - install prefix (default: $PWD/prefix)
#   CMAKE_ARGS   - extra cmake args (optional, space-separated via array eval)
#   EXTRA_PREFIX - CMAKE_PREFIX_PATH entries (optional, colon-separated)
#   JOBS         - parallel jobs
set -euo pipefail

export EM_CACHE="${TMPDIR:-/tmp}/emcache"
mkdir -p "$EM_CACHE"

PREFIX="${PREFIX:-$PWD/prefix}"
mkdir -p "$PREFIX"

if [ -z "${SRC_DIR:-}" ]; then
  echo "error: SRC_DIR required" >&2
  exit 1
fi

NAME="$(basename "$(cd "$SRC_DIR" && pwd)")"
echo "==> static wasm lib: $NAME → $PREFIX"

rm -rf "build-$NAME" "src-$NAME"
cp -a "$SRC_DIR" "src-$NAME"
chmod -R u+w "src-$NAME"
mkdir -p "build-$NAME"
cd "build-$NAME"

CMAKE_PREFIX_ARGS=()
if [ -n "${EXTRA_PREFIX:-}" ]; then
  CMAKE_PREFIX_ARGS+=("-DCMAKE_PREFIX_PATH=${EXTRA_PREFIX}")
fi

# Emscripten defaults FIND_ROOT_PATH_MODE_*=ONLY, which hides host
# CMAKE_PREFIX_PATH packages (logmich/sexp installed as sibling prefixes).
# shellcheck disable=SC2086
emcmake cmake "../src-$NAME" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_INSTALL_PREFIX="$PREFIX" \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_TESTS=OFF \
  -DBUILD_EXTRA=OFF \
  -DWARNINGS=OFF \
  -DWERROR=OFF \
  -DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH \
  -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=BOTH \
  -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=BOTH \
  ${CMAKE_PREFIX_ARGS[@]+"${CMAKE_PREFIX_ARGS[@]}"} \
  ${CMAKE_ARGS:-}

emmake cmake --build . -j"${NIX_BUILD_CORES:-${JOBS:-$(nproc)}}"
emmake cmake --install .

echo "==> $NAME installed under $PREFIX"
ls -la "$PREFIX/lib" 2>/dev/null || true
ls -la "$PREFIX/include" 2>/dev/null || true
