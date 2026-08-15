#!/usr/bin/env bash
# Validate packaging helper layouts without device hardware.
# Creates temporary fake binaries and runs package scripts.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TMP="$(mktemp -d)"
trap 'rm -rf "$TMP"' EXIT

EXAMPLES="$ROOT/examples"
[ -d "$EXAMPLES" ] || { echo "no examples/"; exit 1; }

echo "==> R36S package-port.sh"
fake="$TMP/construo.sdl"
echo '#!/bin/true' > "$fake"
chmod +x "$fake"
"$ROOT/mk/r36s/scripts/package-port.sh" "$fake" "$EXAMPLES" "$TMP/r36s"
test -x "$TMP/r36s/construo.sh"
test -f "$TMP/r36s/controls.txt"
test -d "$TMP/r36s/examples"
grep -q '640x480' "$TMP/r36s/construo.sh"
echo "    ok"

echo "==> Win32 package-zip.sh"
fakeexe="$TMP/construo.sdl.exe"
echo 'MZ' > "$fakeexe"
"$ROOT/mk/win32/scripts/package-zip.sh" "$fakeexe" "$EXAMPLES" "$TMP/win"
test -f "$TMP/win/construo.exe"
test -f "$TMP/win/README.txt"
test -d "$TMP/win/examples"
echo "    ok"

echo "==> WASM scripts present"
test -x "$ROOT/mk/wasm/scripts/build-app.sh"
test -x "$ROOT/mk/wasm/scripts/build-cmake-lib.sh"
test -f "$ROOT/mk/wasm/shell.html"
echo "    ok"

echo "==> Android helpers present"
test -f "$ROOT/mk/android/app/jni/Android.mk"
test -x "$ROOT/mk/android/scripts/install-sdl-libs.sh"
echo "    ok"

echo "All packaging layout checks passed."
