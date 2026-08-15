# TODO.md — Construo port & maintenance tracker

Keep this file current. Mark items `[x]` when done; add new work under the
appropriate section.

---

## Immediate (agent bootstrap)

- [x] Add `AGENTS.md` (project rules, architecture, git/bundle workflow)
- [x] Add this `TODO.md`
- [x] First git bundle: `construo-001-agents-todo.bundle`
- [x] CMake: fix tinycmmc path, geom subdirectory, backend options (`construo-002-…`)

---

## Existing Linux targets (must remain)

- [x] `construo.glut` — OpenGL + GLUT
- [x] `construo.x11` — Xlib software rendering
- [ ] Document both clearly in README after SDL2 lands
- [ ] Optional: desktop `construo-gles2` binary that uses the new GLES2 path
      on Linux for validation (same idea as Pingus `pingusGles2`)

---

## Core port work

### SDL2 backend

- [ ] Introduce CMake options: `CONSTRUO_USE_SDL2`, `CONSTRUO_BACKEND=…`
- [ ] Implement `SDL2Display` (or equivalent) that fulfils
      `RootGraphicContext` + input / system context responsibilities
- [ ] Map SDL events to existing `Action` / key bindings / mouse tools
- [ ] Window title, fullscreen toggle, cursor handling
- [ ] Keep GLUT and X11 backends selectable and working

### GLES2 renderer

- [ ] GLES2 implementation of `GraphicContext` drawing primitives:
  - lines (including width), circles (outline + fill), rects (outline + fill)
  - strings (bitmap or simple texture font; no GLUT bitmap fonts)
  - clipping, clear, flip / swap
- [ ] Minimal shader set (position + color; optional simple text atlas)
- [ ] Works under:
  - Desktop GLES2 (for validation)
  - Emscripten (`FULL_ES2`)
  - Android (SDL2 + GLES2)
  - R36S (SDL2 + GLES2)

### Platform-specific packaging (Nix + helpers)

Reference: `pingus` repo (`nix/*.nix`, `mk/wasm/`, `mk/android/`, `mk/r36s/`).

#### WASM (Emscripten)

- [ ] `nix/wasm.nix` (or equivalent flake output `.#construo-wasm`)
- [ ] SDL2 (and minimal deps) for `wasm32-emscripten`
- [ ] Shell HTML / preload of `examples/` (and any other data)
- [ ] Main-loop integration (`emscripten_set_main_loop` or SDL equivalent)
- [ ] Build scripts under `mk/wasm/` adapted from Pingus

#### Android

- [ ] `nix/android.nix` / APK packaging helpers under `mk/android/`
- [ ] SDL2 Android project skeleton (JNI, manifest, icons)
- [ ] GLES2 path verified on device / emulator
- [ ] Touch input mapped to existing tools (reasonable defaults)

#### Win32 / Win64 (MinGW cross)

- [ ] Flake outputs `.#construo-win32` / `.#construo-win64` (or flat packages)
- [ ] SDL2 MinGW packages (reuse grumnix / Pingus pattern where possible)
- [ ] No X11 / GLUT on Windows; SDL2 + GLES2 (or desktop GL if preferred for Win)
- [ ] Installer / zip layout consistent with Pingus style if practical

#### R36S (ArkOS)

- [ ] `nix/r36s.nix` + toolchain files under `mk/r36s/`
- [ ] Cross sysroot / toolchain (aarch64 and/or armhf as needed)
- [ ] SDL2 + GLES2 binary suitable for the handheld
- [ ] Packaging notes (where to place binary + examples)

---

## Dependencies & CMake hygiene

- [x] Clean up duplicate / broken `find_package` / `add_subdirectory` blocks in
      `CMakeLists.txt` (prio/geom fixed; tinycmmc path corrected)
- [ ] Ensure `tinycmmc` module path and `external/` fallbacks work for all
      new targets (including Emscripten FIND_ROOT)
- [ ] glm, libsigc++-3 (or 2 if forced by platform), fmt — available for
      WASM / Android / MinGW / R36S as required
- [ ] Optional: drop or gate `xdgcpp` on platforms where it does not apply
- [ ] Prefer system packages on native Linux; vendor only when necessary

---

## Game / content (lower priority while ports land)

See also the historical `TODO` file in the repo root for older ideas.

- [ ] Prefabs / object groups
- [ ] Limited-resource / mission-style scenarios
- [ ] Better font rendering (replace GLUT bitmap fonts on all backends)
- [ ] Optional sound (not required for first ports)
- [ ] Binary construction format (performance)

---

## Documentation

- [ ] Update `README.md` with SDL2 / GLES2 build instructions and port status
- [ ] Note in `NEWS` when first WASM / Android / Win / R36S builds ship
- [ ] Keep `AGENTS.md` and this file in sync with reality

---

## Done recently (bootstrap)

- Repository already has working X11 and GLUT backends and a Nix flake for
  native Linux.
- Helper libraries under `external/` as submodules.
