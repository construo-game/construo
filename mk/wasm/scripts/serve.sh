#!/usr/bin/env bash
# Serve a Construo wasm build directory over HTTP.
# Env: APP_NAME (default: construo), CONSTRUO_WASM_PORT (default 8765), PKG, BROWSER
set -euo pipefail

if [ -n "${PKG:-}" ]; then
  cd "$PKG"
fi

app_name="${APP_NAME:-construo}"
port="${CONSTRUO_WASM_PORT:-8765}"

if [ ! -f "${app_name}.html" ] && [ ! -f index.html ]; then
  echo "error: no ${app_name}.html or index.html in $(pwd)" >&2
  echo "  build with mk/wasm/scripts/build-app.sh first" >&2
  exit 1
fi

echo "Serving $(pwd) at http://127.0.0.1:${port}/"
if command -v python3 >/dev/null; then
  exec python3 -m http.server "$port" --bind 127.0.0.1
else
  exec python -m SimpleHTTPServer "$port"
fi
