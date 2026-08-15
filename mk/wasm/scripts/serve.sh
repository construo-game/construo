#!/usr/bin/env bash
# Serve a Construo wasm build directory over HTTP and open a browser.
# Env: APP_NAME (default: construo), PKG, CONSTRUO_WASM_PORT (default 8765),
#      CONSTRUO_WASM_OPEN_QUERY, BROWSER
# Aligns with pingus/mk/wasm/scripts/serve.sh (no-store cache headers + open).
set -euo pipefail

cli_query=()
while [ "$#" -gt 0 ]; do
  case "$1" in
    --debug)   cli_query+=("debug=1"); shift ;;
    --verbose|-v) cli_query+=("verbose=1"); shift ;;
    --help|-h)
      echo "Usage: serve.sh [--debug] [--verbose]"
      echo "  --debug / --verbose  append query flags for the shell"
      echo "Env: APP_NAME, PKG, CONSTRUO_WASM_PORT, CONSTRUO_WASM_OPEN_QUERY, BROWSER"
      exit 0
      ;;
    --) shift; break ;;
    -*)
      echo "error: unknown option: $1 (try --help)" >&2
      exit 1
      ;;
    *)
      echo "error: unexpected argument: $1 (try --help)" >&2
      exit 1
      ;;
  esac
done

if [ -n "${PKG:-}" ]; then
  cd "$PKG"
fi

app_name="${APP_NAME:-construo}"
port="${CONSTRUO_WASM_PORT:-8765}"

html=
for candidate in "${app_name}.html" "${app_name}.sdl.html" index.html; do
  if [ -f "$candidate" ]; then
    html="$candidate"
    break
  fi
done
if [ -z "$html" ]; then
  echo "error: no ${app_name}.html / ${app_name}.sdl.html / index.html in $(pwd)" >&2
  echo "  build with mk/wasm/scripts/build-app.sh or nix build .#construo-wasm first" >&2
  exit 1
fi

port_file=$(mktemp)
server_pid=
trap 'kill "$server_pid" 2>/dev/null || true; rm -f "$port_file"' EXIT

# Always no-store so a rebuilt .js/.wasm/.data is not kept after a frozen-tab
# reload (same rationale as Pingus).
python3 -c '
import http.server, socketserver, sys
port_file, port = sys.argv[1], int(sys.argv[2])

class NoCacheHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0")
        self.send_header("Pragma", "no-cache")
        self.send_header("Expires", "0")
        super().end_headers()
    def log_message(self, *a):
        pass

socketserver.TCPServer.allow_reuse_address = True
try:
    httpd = socketserver.TCPServer(("127.0.0.1", port), NoCacheHandler)
except OSError as e:
    sys.stderr.write(
        "error: cannot bind 127.0.0.1:%s (%s)\n"
        "       set CONSTRUO_WASM_PORT to a free port\n" % (port, e))
    sys.exit(1)
open(port_file, "w").write(str(httpd.server_address[1]))
httpd.serve_forever()
' "$port_file" "$port" &
server_pid=$!

for i in $(seq 1 50); do
  [ -s "$port_file" ] && break
  sleep 0.05
done
if [ ! -s "$port_file" ]; then
  echo "error: local HTTP server failed to start on port $port" >&2
  exit 1
fi
port=$(cat "$port_file")

query="${CONSTRUO_WASM_OPEN_QUERY:-}"
if [ "${#cli_query[@]}" -gt 0 ]; then
  joined=$(IFS='&'; echo "${cli_query[*]}")
  if [ -n "$query" ]; then
    query="${query}&${joined}"
  else
    query="$joined"
  fi
fi

url="http://127.0.0.1:${port}/${html}"
if [ -n "$query" ]; then
  url="${url}?${query}"
fi

echo "Serving $(pwd) at ${url}"
echo "Cache-Control: no-store (hard-reload picks up new .js/.wasm/.data)"

open_browser() {
  local u="$1"
  if [ -n "${BROWSER:-}" ]; then
    "$BROWSER" "$u" >/dev/null 2>&1 && return 0
  fi
  if command -v xdg-open >/dev/null 2>&1; then
    xdg-open "$u" >/dev/null 2>&1 && return 0
  fi
  if command -v open >/dev/null 2>&1; then
    open "$u" >/dev/null 2>&1 && return 0
  fi
  echo "Open this URL in a browser: $u"
  return 1
}

open_browser "$url" || true

wait "$server_pid"
