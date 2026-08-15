#!/usr/bin/env bash
# Collect a MinGW-built construo.sdl + runtime DLLs + examples into a zip-friendly tree.
# Usage: package-zip.sh /path/to/construo.sdl[.exe] /path/to/examples /path/to/outdir
set -euo pipefail

BIN="${1:?construo.sdl or construo.sdl.exe}"
EXAMPLES="${2:?examples dir}"
OUT="${3:?outdir}"
mkdir -p "$OUT"

if [[ "$BIN" == *.exe ]]; then
  cp -a "$BIN" "$OUT/construo.exe"
else
  cp -a "$BIN" "$OUT/construo.exe"
fi
cp -a "$EXAMPLES" "$OUT/examples"

PREFIX="${MINGW_PREFIX:-/usr/x86_64-w64-mingw32}"
BIN_DIR="$(cd "$(dirname "$BIN")" && pwd)"

# Runtime DLLs commonly required next to the executable.
DLLS=(
  SDL2.dll
  libgcc_s_seh-1.dll
  libstdc++-6.dll
  libwinpthread-1.dll
  libssp-0.dll
  libGLESv2.dll
  libEGL.dll
  libglapi-0.dll
)

copy_dll() {
  local dll="$1"
  local found=""
  for dir in "$BIN_DIR" "$PREFIX/bin" "${SDL2_BIN:-}" ; do
    [ -n "$dir" ] || continue
    if [ -f "$dir/$dll" ]; then
      found="$dir/$dll"
      break
    fi
  done
  if [ -n "$found" ]; then
    cp -a "$found" "$OUT/"
    echo "  + $dll"
  fi
}

echo "Copying runtime DLLs (missing ones are skipped)..."
for dll in "${DLLS[@]}"; do
  copy_dll "$dll"
done

cat > "$OUT/README.txt" <<'TXT'
Construo (Win64 SDL2 + GLES2)

Run construo.exe. Example constructions are under examples/.

If the window fails to open with a GL/GLES error, install a recent GPU
driver. The binary links against opengl32 for the desktop compatibility
fallback; some setups also ship ANGLE (libGLESv2.dll / libEGL.dll).

Controller input uses the SDL2 game controller API (see PORTING.md).
TXT

echo "Win32/Win64 layout ready in $OUT"
