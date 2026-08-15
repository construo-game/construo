#!/usr/bin/env bash
# Push example constructions to a connected Android device (adb).
# Uses external storage path reported by the package, or a conventional fallback.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
PKG="${CONSTRUO_ANDROID_PACKAGE:-org.construo.game}"
DEST="${1:-/sdcard/Android/data/${PKG}/files/examples}"

if ! command -v adb >/dev/null; then
  echo "error: adb not found" >&2
  exit 1
fi

echo "Pushing $ROOT/examples → device:$DEST"
adb shell mkdir -p "$DEST"
adb push "$ROOT/examples/." "$DEST/"
echo "Done. Launch Construo; examples should appear under the file browser."
