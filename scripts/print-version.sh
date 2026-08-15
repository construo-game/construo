#!/usr/bin/env bash
# Print the development version string the same way the flake does.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
BASE="$(tr -d '\n' < "$ROOT/VERSION")"
if [[ "$BASE" != *"-dev"* ]]; then
  echo "$BASE"
  exit 0
fi
COUNT="$(git -C "$ROOT" rev-list --count HEAD 2>/dev/null || echo 0)"
SHORT="$(git -C "$ROOT" rev-parse --short HEAD 2>/dev/null || echo unknown)"
DIRTY=""
if [[ -n "$(git -C "$ROOT" status --porcelain 2>/dev/null || true)" ]]; then
  DIRTY="-dirty"
fi
echo "${BASE}.${COUNT}+g${SHORT}${DIRTY}"
