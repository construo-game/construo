# WASM (Emscripten) helpers for Construo

Adapted from the Pingus layout (`mk/wasm/`).

| Path | Role |
|------|------|
| `shell.html` | Browser shell (canvas, load status, version footer) |
| `scripts/build-app.sh` | `emcmake` configure + build with SDL2-only flags |

## Expected environment

Set by `nix/wasm.nix` (or manually):

- `SRC_DIR` — repository root
- `SDL_WASM_LIBS` — prefix with SDL2 built for `wasm32-emscripten`
- `EXAMPLES_DIR` — constructions to `--preload-file` at `/examples`
- `PROJECT_VERSION_FULL`, `SOURCE_URL` — stamped into `shell.html`

## CMake flags used

```
-DCONSTRUO_USE_SDL2=ON
-DCONSTRUO_USE_GLUT=OFF
-DCONSTRUO_USE_X11=OFF
```

The game uses the GLES2 path under WebGL (`FULL_ES2`). The SDL2 display
registers an Emscripten main loop when `__EMSCRIPTEN__` is defined.

## Status

`nix/wasm.nix` builds static SDL2, zlib, libsigc++-2, glm headers, and the
vendored helpers (logmich, sexpcpp, geom, priocpp) for wasm32-emscripten
and exposes `construo-wasm`. Local testing works via the scripts below.

## Scripts

| Script | Role |
|--------|------|
| `scripts/build-sdl2.sh` | Static SDL2 for wasm (`SDL_SRC`, `PREFIX`) |
| `scripts/build-zlib.sh` | Static zlib for wasm (`ZLIB_SRC`) |
| `scripts/build-cmake-lib.sh` | Static CMake helper lib (`SRC_DIR`, `PREFIX`) |
| `scripts/build-app.sh` | emcmake configure + build |
| `scripts/serve.sh` | HTTP server for local browser test |

Example:

```bash
export SDL_SRC=/path/to/SDL2
export PREFIX=$PWD/wasm-prefix
mk/wasm/scripts/build-sdl2.sh
SDL_WASM_LIBS=$PREFIX mk/wasm/scripts/build-app.sh
cd build-wasm && ../mk/wasm/scripts/serve.sh
```
