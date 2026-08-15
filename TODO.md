# TODO.md — Construo port & maintenance tracker

Keep this file current. Mark items `[x]` when done; add new work under the
appropriate section.

---

## Immediate (agent bootstrap)

- [x] Add `AGENTS.md` (project rules, architecture, git/bundle workflow)
- [x] Single-source `VERSION` → CMake `PROJECT_VERSION_FULL` / `CONSTRUO_VERSION` + flake
- [x] Add this `TODO.md`
- [x] Bundles `construo-001` … `construo-003` (agents, CMake options, TODO)
- [x] Inline tinycmmc CMake helpers under `cmake/` (no top-level tinycmmc dep)
- [x] SDL2 display + GLES2 renderer (desktop `construo.sdl`)
- [x] Nix: `construo` + `construo-sdl` + `construo-all`; `nix flake check` builds them
- [x] Nix: `devShells.default` for local CMake work
- [x] Nix: `nix/` + `mk/` scaffolding for cross ports

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
- [x] `mk/wasm/scripts/build-sdl2.sh` + `serve.sh` for local wasm toolchain
- [x] `mk/wasm/scripts/build-zlib.sh` (static zlib for wasm)
- [x] `nix/wasm.nix`: glmPrefix, sigcWasm, zlibWasmLibs, sdl2WasmLibs, construo-wasm derivation
- [x] Flake input `sdl2-src` (SDL 2.30.3) and `packages.construo-wasm` wiring
- [x] Static wasm builds of helpers (`logmich`, `sexpcpp`, `geom`, `priocpp`) via `build-cmake-lib.sh`
- [x] `sdlWasmLibs` joins SDL2 + zlib + sigc++ + helpers for CMAKE_PREFIX_PATH
- [ ] Runtime validation in browser / full Emscripten link dry-run

#### Android

- [x] `nix/android.nix` + `mk/android/` skeleton (manifest, Android.mk, scripts)
- [x] Touch input (finger → PRIMARY/SECONDARY)
- [x] SDL2 game controller mapping (handheld / R36S)
- [x] HiDPI mouse/touch → drawable coordinate mapping
- [x] Android.mk source list + Application.mk (NDK staging still manual)
- [x] Android `package-apk.sh` (needs NDK + staged SDL2)
- [x] Gradle skeleton (`mk/android/*.gradle`) + `build-apk.sh`
- [x] Android.mk includes vendored external sources (logmich, sexp, prio sexp path)
- [x] Flake `construo-android` / `construo-r36s` helper packages (full APK/device binary still external)
- [x] `mk/android/scripts/install-sdl-libs.sh` to stage SDL2 under jni/
- [x] Android default fullscreen when `__ANDROID__`
- [x] SDL app lifecycle: pause sim on background, ESC on terminate
- [x] WASM shell: canvas click-to-focus for keyboard input
- [x] Android datadir paths (internal/external storage) + soft-fail without examples
- [x] Android soft-fail sets `path_manager` base to internal storage
- [x] Stage examples into APK assets; `push-examples.sh` for adb deploy
- [x] Align package id `org.construo.game`; stamp VERSION; optional release signing env
- [x] CMake preset `mingw-win64` for MinGW cross configures
- [ ] Full signed APK release flow / CI artifact (needs keystore + NDK CI image)

#### Win32 / Win64 (MinGW)

- [x] `Win32System` (`src/win32_system.*`) and CMake WIN32 defaults
- [x] Portable `path_manager` (no POSIX `access()`)
- [x] UnixSystem HOME fallbacks for Emscripten/Android
- [x] `nix/win32.nix` / `mk/win32/` scaffolding
- [x] Win32 `package-zip.sh` layout helper
- [x] `mk/win32/scripts/cross-cmake.sh` MinGW configure helper
- [x] Flake input `SDL2-win32` + `mkLibs` for `pkgsCross.mingwW64`
- [x] `packages.construo-win64` / `construo-win64-bin` (cross derivation)
- [x] `CONSTRUO_TARGET_WINDOWS` unifies native and cross MinGW CMake paths
- [x] `package-zip.sh` copies MinGW + optional GLES/ANGLE DLLs
- [x] `SDL_MAIN_HANDLED` + MinGW/SDL2main/opengl32 link order for Win targets
- [ ] Validate Win64 link (GLESv2 / ANGLE / system GL) on a real MinGW run

#### R36S (ArkOS)

- [x] `nix/r36s.nix` + aarch64/armhf toolchains under `mk/r36s/`
- [x] `CROSSCOMPILE.md` sysroot notes
- [x] `mk/r36s/scripts/package-port.sh` layout helper
- [x] R36S launcher defaults (`-f -g 640x480`), controls.txt, README in package
- [ ] Device package layout validation on hardware

---

## Dependencies & CMake hygiene

- [x] Convert `external/*` from git submodules to squashed subtrees (latest master)
- [x] Vendor header-only glm 1.0.1 under `external/glm` (platforms = all, MinGW-safe)
- [x] Replace `{fmt}` with C++20 `std::format` + optional `print.hpp` polyfill
- [x] Flake builds helpers from vendored `external/*` (no remote helper flake inputs)

- [x] Fix geom subdirectory / tinycmmc module path
- [x] Remove top-level tinycmmc dependency; inline under `cmake/`
- [x] Emscripten FIND_ROOT_PATH_MODE BOTH for package/include/library
- [x] CMake accepts sigc++-3 or 2; glm via prefix (still need wasm builds of deps)
- [x] Gate `xdgcpp` via `CONSTRUO_NO_XDGCPP` (+ unix_system fallback)
- [x] Prefer system packages on native Linux (`find_package` / pkg-config)
- [x] Fix `-g/--geometry` parse (`WIDTHxHEIGHT` required correctly)
- [x] `scripts/check-port-layouts.sh` smoke-tests packaging helpers without hardware
- [x] Expand layout checks (stamp-version, CMake presets, flake package names)
- [x] `scripts/refresh-flake-lock.sh` (+ AGENTS note: run after input changes)
- [x] `CONSTRUO_TARGET_{WINDOWS,ANDROID,EMSCRIPTEN}` unified platform detection
- [x] `scripts/ci-smoke.sh` for CI without Nix/NDK/EMSDK

---

## Documentation

- [x] `PORTING.md` overview of SDL2/GLES2 targets
- [x] `CMakePresets.json` for common configure profiles

- [x] Update `README.md` with SDL2 / GLES2 build instructions
- [x] Note in `NEWS` (ports in progress); update again when packages ship
- [x] Keep `AGENTS.md` and this file in sync with reality
- [x] GitHub Actions `nix flake check` workflow
