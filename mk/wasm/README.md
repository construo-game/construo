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

Scaffolding is in place; a full flake output `.#construo-wasm` still needs a
prebuilt SDL2 wasm prefix and dependency wiring (fmt, glm, sigc++, priocpp, …)
similar to Pingus `nix/wasm.nix`.
