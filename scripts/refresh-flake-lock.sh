#!/usr/bin/env bash
# Regenerate flake.lock after flake.nix input changes.
# Requires Nix. Safe to re-run.
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"
if ! command -v nix >/dev/null; then
  echo "error: nix is required to refresh flake.lock" >&2
  exit 1
fi
echo "==> nix flake lock (update all inputs)"
nix flake lock --update-input nixpkgs --update-input flake-utils \
  --update-input SDL2-win32 2>/dev/null || nix flake update
echo "==> current inputs:"
nix flake metadata 2>/dev/null | head -40 || true
echo "Done. Commit the updated flake.lock."
