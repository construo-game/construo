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
- [x] XBM tool cursors for SDL2 (same assets as X11)
- [x] Keep GLUT and X11 backends selectable and working

### GLES2 renderer

- [x] GLES2 implementation of drawing primitives (lines, circles, rects, fill)
- [x] Shader set (position + color + optional texture)
- [x] Simple 8×8 bitmap font atlas for `draw_string`
- [x] Clipping (scissor), clear, flip via SDL swap
- [ ] Validate on Emscripten (`FULL_ES2`), Android, R36S
- [x] Complete 8×8 ASCII bitmap font atlas for GLES2
- [ ] Optional TTF/denser atlas later if needed

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
- [x] HiDPI mouse/touch → drawable coordinate mapping
- [x] Android.mk source list + Application.mk (NDK staging still manual)
- [x] Android `package-apk.sh` (needs NDK + staged SDL2)
- [ ] Full Gradle/APK release flow

#### Win32 / Win64 (MinGW)

- [x] `Win32System` (`src/win32_system.*`) and CMake WIN32 defaults
- [x] Portable `path_manager` (no POSIX `access()`)
- [x] `nix/win32.nix` / `mk/win32/` scaffolding (SDL2-win32 input when package lands)
- [x] Win32 `package-zip.sh` layout helper
- [ ] Cross-build helper libs + real `.#construo-win64` package

#### R36S (ArkOS)

- [x] `nix/r36s.nix` + aarch64/armhf toolchains under `mk/r36s/`
- [x] `CROSSCOMPILE.md` sysroot notes
- [x] `mk/r36s/scripts/package-port.sh` layout helper
- [ ] Device package layout validation on hardware

---

## Dependencies & CMake hygiene

- [x] Fix geom subdirectory / tinycmmc module path
- [x] Remove top-level tinycmmc dependency; inline under `cmake/`
- [x] Emscripten FIND_ROOT_PATH_MODE BOTH for package/include/library
- [x] CMake accepts sigc++-3 or 2; glm/fmt via prefix (still need wasm builds of deps)
- [x] Gate `xdgcpp` via `CONSTRUO_NO_XDGCPP` (+ unix_system fallback)
- [x] Prefer system packages on native Linux (`find_package` / pkg-config)

---

## Documentation

- [x] `PORTING.md` overview of SDL2/GLES2 targets
- [x] `CMakePresets.json` for common configure profiles

- [x] Update `README.md` with SDL2 / GLES2 build instructions
- [x] Note in `NEWS` (ports in progress); update again when packages ship
- [x] Keep `AGENTS.md` and this file in sync with reality
