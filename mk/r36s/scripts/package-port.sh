#!/usr/bin/env bash
# Layout an ArkOS / R36S "Ports" directory for Construo.
# Usage: package-port.sh /path/to/construo.sdl /path/to/examples /path/to/outdir
set -euo pipefail

BIN="${1:?construo.sdl binary}"
EXAMPLES="${2:?examples directory}"
OUT="${3:?output directory}"

mkdir -p "$OUT"
cp -a "$BIN" "$OUT/construo.sdl"
chmod +x "$OUT/construo.sdl"
cp -a "$EXAMPLES" "$OUT/examples"

# Launcher: native 640×480, fullscreen, kmsdrm when available.
cat > "$OUT/construo.sh" <<'SH'
#!/bin/bash
cd "$(dirname "$0")"
export SDL_VIDEODRIVER="${SDL_VIDEODRIVER:-kmsdrm}"
# R36S panel is 640×480; force geometry unless the user overrides.
# examples/ is packaged next to this script
exec ./construo.sdl --datadir "$(pwd)" -f -g 640x480 "$@"
SH
chmod +x "$OUT/construo.sh"

cat > "$OUT/controls.txt" <<'TXT'
Construo — R36S (SNES-style) controls

  Left stick        software cursor
  Right stick       scroll the view
  A                 primary (place / select)
  B                 secondary (remove / rotate)
  X                 fix particle / selection
  Y (hold)          quick menu — stick:
                      Up Load, Down Save, Left Clear, Right Slow-mo
  Start             run / stop simulation
  Select            show / hide on-screen UI
  D-pad             tool mode: U Insert, D Select, L Collider, R Zoom
  L / R shoulder    zoom out / in
  L / R trigger     undo / redo
  L-stick click     delete
  R-stick click     toggle grid

Quit: keyboard Esc, or close via OS / PortMaster hotkey.
TXT

cat > "$OUT/README.txt" <<'TXT'
Construo on R36S (ArkOS)

1. Copy this folder to a Ports location (e.g. /roms/ports/construo/).
2. Run construo.sh (or construo.sdl -f -g 640x480).
3. Examples live under examples/.

Binary must be built against a sysroot matching the device glibc
(see mk/r36s/CROSSCOMPILE.md).
TXT

echo "Packaged Construo for R36S-style layout in $OUT"
echo "  construo.sdl  construo.sh  controls.txt  README.txt  examples/"
