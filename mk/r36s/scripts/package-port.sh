#!/usr/bin/env bash
# Layout a simple R36S/ArkOS "port" directory for Construo.
# Usage: package-port.sh /path/to/construo.sdl /path/to/examples /path/to/outdir
set -euo pipefail

BIN="${1:?construo.sdl binary}"
EXAMPLES="${2:?examples directory}"
OUT="${3:?output directory}"

mkdir -p "$OUT"
cp -a "$BIN" "$OUT/construo.sdl"
chmod +x "$OUT/construo.sdl"
cp -a "$EXAMPLES" "$OUT/examples"

cat > "$OUT/construo.sh" <<'SH'
#!/bin/bash
cd "$(dirname "$0")"
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-kmsdrm}"
exec ./construo.sdl "$@"
SH
chmod +x "$OUT/construo.sh"

echo "Packaged Construo for R36S-style layout in $OUT"
echo "  construo.sdl, construo.sh, examples/"
