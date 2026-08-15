#!/usr/bin/env bash
# Stamp VERSION into AndroidManifest.xml (gradle also reads VERSION / CONSTRUO_VERSION).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
VER="$(tr -d '\n' < "$ROOT/VERSION")"
MANIFEST="$ROOT/mk/android/app/AndroidManifest.xml"
if [ ! -f "$MANIFEST" ]; then
  echo "error: missing $MANIFEST" >&2
  exit 1
fi
# portable sed: rewrite android:versionName="..."
tmp="$(mktemp)"
sed -E "s/android:versionName=\"[^\"]*\"/android:versionName=\"${VER}\"/" "$MANIFEST" > "$tmp"
mv "$tmp" "$MANIFEST"
echo "Stamped AndroidManifest versionName=$VER"
export CONSTRUO_VERSION="$VER"
