# TODO.md — Construo port & maintenance tracker

Keep this file current. Mark items `[x]` when done; add new work under the
appropriate section.

---

## Immediate (agent bootstrap)

- [x] Add `AGENTS.md` (project rules, architecture, git/bundle workflow)
- [x] Add this `TODO.md`
- [x] Bundles `construo-001` … `construo-003` (agents, CMake options, TODO)
- [x] Inline tinycmmc CMake helpers under `cmake/` (no top-level tinycmmc dep)
- [x] SDL2 display + GLES2 renderer (desktop `construo.sdl`)
- [x] Nix: `construo` + `construo-sdl` packages; `nix/` + `mk/` scaffolding

---

## Existing Linux targets (must remain)

- [x] `construo.glut` — OpenGL + GLUT
- [x] `construo.x11` — Xlib software rendering
- [x] `construo.sdl` — SDL2 + GLES2 (desktop validation)
- [x] Document all three clearly in README
- [x] Optional desktop install of `.desktop` for `construo.sdl`

---

## Core port work

### SDL2 backend

- [x] CMake options: `CONSTRUO_USE_SDL2`, `CONSTRUO_USE_GLUT`, `CONSTRUO_USE_X11`
- [x] `SDL2Display` implementing `RootGraphicContext` + `InputContext`
- [x] Map SDL events to existing `Action` / mouse tools
- [x] Window title, fullscreen toggle (basic)
- [x] System cursors mapped for SDL2 (bitmap cursors still optional)
- [x] Keep GLUT and X11 backends selectable and working

### GLES2 renderer

- [x] GLES2 implementation of drawing primitives (lines, circles, rects, fill)
- [x] Shader set (position + color + optional texture)
- [x] Simple 8×8 bitmap font atlas for `draw_string`
- [x] Clipping (scissor), clear, flip via SDL swap
- [ ] Validate on Emscripten (`FULL_ES2`), Android, R36S
- [ ] Improve font (TTF or denser atlas) if needed

### Platform packaging

#### WASM (Emscripten)

- [x] `mk/wasm/shell.html`, `scripts/build-app.sh`, README
- [x] `nix/wasm.nix` scaffolding (glm prefix notes, next steps)
- [x] Emscripten main loop in `SDL2Display::run()`
- [x] CMake `EMSCRIPTEN` link options (FULL_ES2, shell, preload examples)
- [ ] Prebuilt SDL2 + static deps for wasm; flake output `.#construo-wasm`
- [ ] Runtime validation in browser

#### Android

- [x] `nix/android.nix` + `mk/android/` skeleton (manifest, Android.mk, scripts)
- [x] Touch input (finger → PRIMARY/SECONDARY)
- [ ] Full APK packaging / NDK source staging

#### Win32 / Win64 (MinGW)

- [x] `Win32System` (`src/win32_system.*`) and CMake WIN32 defaults
- [x] Flake input `SDL2-win32` + `nix/win32.nix` / `mk/win32/` scaffolding
- [ ] Cross-build helper libs + real `.#construo-win64` package

#### R36S (ArkOS)

- [x] `nix/r36s.nix` + aarch64/armhf toolchains under `mk/r36s/`
- [x] `CROSSCOMPILE.md` sysroot notes
- [ ] Device package layout validation

---

## Dependencies & CMake hygiene

- [x] Fix geom subdirectory / tinycmmc module path
- [x] Remove top-level tinycmmc dependency; inline under `cmake/`
- [ ] Ensure helpers work under Emscripten FIND_ROOT
- [ ] glm, libsigc++-3, fmt for WASM / Android / MinGW / R36S
- [x] Gate `xdgcpp` via `CONSTRUO_NO_XDGCPP` (+ unix_system fallback)
- [ ] Prefer system packages on native Linux

---

## Documentation

- [x] Update `README.md` with SDL2 / GLES2 build instructions
- [ ] Note in `NEWS` when first WASM / Android / Win / R36S builds ship
- [x] Keep `AGENTS.md` and this file in sync with reality
