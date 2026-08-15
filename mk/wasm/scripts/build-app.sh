#!/usr/bin/env bash
# Cross-compile Construo to wasm32 + HTML via emcmake.
# Environment (typically set by nix/wasm.nix):
#   APP_NAME, SRC_DIR, SDL_WASM_LIBS, EXAMPLES_DIR, PROJECT_VERSION_FULL,
#   GIT_REV, SOURCE_URL, CMAKE_VERBOSE
set -euo pipefail

export EM_CACHE="${TMPDIR:-/tmp}/emcache"
mkdir -p "$EM_CACHE"

APP_NAME="${APP_NAME:-construo}"
SRC_DIR="${SRC_DIR:-.}"
SDL_PREFIX="${SDL_WASM_LIBS:-}"
EXAMPLES_DIR="${EXAMPLES_DIR:-${SRC_DIR}/examples}"
VERSION_FULL="${PROJECT_VERSION_FULL:-unknown}"
SOURCE_URL="${SOURCE_URL:-https://github.com/construo-game/construo}"
SHELL_HTML="${SHELL_HTML:-${SRC_DIR}/mk/wasm/shell.html}"

BUILD_DIR="${BUILD_DIR:-build-wasm}"
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

CMAKE_ARGS=(
  -DCMAKE_BUILD_TYPE=Release
  -DCONSTRUO_USE_SDL2=ON
  -DCONSTRUO_USE_GLUT=OFF
  -DCONSTRUO_USE_X11=OFF
  -DBUILD_TESTS=OFF
  -DWARNINGS=ON
)

if [ -n "$SDL_PREFIX" ]; then
  CMAKE_ARGS+=(
    "-DCMAKE_PREFIX_PATH=${SDL_PREFIX}"
    "-DSDL2_DIR=${SDL_PREFIX}/lib/cmake/SDL2"
  )
fi

# Preload examples into the virtual FS at /examples
PRELOAD_FLAGS=()
if [ -d "$EXAMPLES_DIR" ]; then
  PRELOAD_FLAGS+=(--preload-file "${EXAMPLES_DIR}@/examples")
fi

# Substitute version tokens into a working shell.html copy
WORK_SHELL="$(pwd)/shell.html"
sed \
  -e "s|@versionFull@|${VERSION_FULL}|g" \
  -e "s|@sourceUrl@|${SOURCE_URL}|g" \
  "$SHELL_HTML" > "$WORK_SHELL"

echo "==> emcmake configure"
emcmake cmake "$SRC_DIR" "${CMAKE_ARGS[@]}"

echo "==> build"
if [ "${CMAKE_VERBOSE:-0}" = "1" ]; then
  cmake --build . --verbose
else
  cmake --build .
fi

# Locate the binary (construo.sdl or construo)
BIN=""
for candidate in construo.sdl construo; do
  if [ -f "$candidate" ] || [ -f "${candidate}.js" ]; then
    BIN="$candidate"
    break
  fi
done
if [ -z "$BIN" ]; then
  echo "ERROR: no construo binary found in $(pwd)" >&2
  ls -la
  exit 1
fi

echo "==> link with shell + preload (if not already done by CMake)"
# When CMake already produced .js/.wasm, optionally re-link with explicit flags.
if [ -f "${BIN}.wasm" ] || [ -f "${APP_NAME}.wasm" ]; then
  echo "WASM artifact present."
else
  echo "NOTE: ensure target_link_options include FULL_ES2 and shell file for browser builds."
fi

echo "==> done (build dir: $(pwd))"
