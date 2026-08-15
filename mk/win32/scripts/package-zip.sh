#!/usr/bin/env bash
# Collect a MinGW-built construo.sdl + DLLs + examples into a zip-friendly tree.
# Usage: package-zip.sh /path/to/construo.sdl /path/to/examples /path/to/outdir
set -euo pipefail
BIN="${1:?construo.sdl}"
EXAMPLES="${2:?examples dir}"
OUT="${3:?outdir}"
mkdir -p "$OUT"
cp -a "$BIN" "$OUT/construo.exe"
cp -a "$EXAMPLES" "$OUT/examples"
# Copy runtime DLLs that live next to the binary or in MINGW_PREFIX/bin
PREFIX="${MINGW_PREFIX:-/usr/x86_64-w64-mingw32}"
for dll in SDL2.dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
  if [[ -f "$(dirname "$BIN")/$dll" ]]; then
    cp -a "$(dirname "$BIN")/$dll" "$OUT/"
  elif [[ -f "$PREFIX/bin/$dll" ]]; then
    cp -a "$PREFIX/bin/$dll" "$OUT/"
  fi
done
cat > "$OUT/README.txt" <<'TXT'
Construo (Win32/Win64 SDL2 build)
Run construo.exe. Example constructions are under examples/.
TXT
echo "Win32 layout ready in $OUT"
