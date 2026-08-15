# Porting Construo (SDL2 + GLES2)

The existing **X11** and **GLUT/OpenGL** backends remain. New targets share one
path: **SDL2** for window/input and **GLES2** for drawing (`GLES2Renderer`).

| Target | Entry | Notes |
|--------|--------|--------|
| Linux desktop (validation) | `construo.sdl` | Try GLES2 context, then GL 2.1 compatibility |
| WASM | `mk/wasm/`, `nix/wasm.nix` | Emscripten main loop; `FULL_ES2`; preload `examples/` |
| Android | `mk/android/` | `SDLActivity`, touch → PRIMARY/SECONDARY |
| Win32/Win64 | `Win32System`, `mk/win32/` | `%APPDATA%/construo`; MinGW + SDL2 |
| R36S / ArkOS | `mk/r36s/` | Sysroot required (old glibc); aarch64 preferred |

## Build switches

```text
-DCONSTRUO_USE_SDL2=ON|OFF
-DCONSTRUO_USE_GLUT=ON|OFF
-DCONSTRUO_USE_X11=ON|OFF
-DCONSTRUO_NO_XDGCPP=ON|OFF   # auto ON on Win/Android/Emscripten
```

## Architecture

```text
GraphicContext / RootGraphicContext / InputContext
        ↑
   SDL2Display  ──→  GLES2Renderer (shaders, VBO, font atlas)
        ↑
   ConstruoMain  +  UnixSystem | Win32System
```

## Status

| Area | State |
|------|--------|
| SDL2 + GLES2 runtime | Done (desktop validation via `construo.sdl`) |
| external/ subtrees + std::format | Done |
| Flake native packages | Done |
| construo-wasm | Derivation + static helpers; needs EMSDK dry-run |
| construo-win64 | Cross derivation; needs MinGW runtime check |
| Android NDK/Gradle | Scripts + lifecycle + examples paths; needs device APK |
| R36S | Toolchain + package layout; needs device |

Tracked in `TODO.md`.


Reference: [Pingus](https://github.com/pingus/pingus) `nix/` and `mk/` layouts.

## CMake presets

```bash
cmake --preset linux-sdl2
cmake --build --preset linux-sdl2
```

Also: `linux-x11-glut`, `linux-all`, `emscripten` (requires EMSDK).

## Game controllers (handheld / TV)

`SDL2Display` opens the first SDL game controller. Default mapping:

| Control | Action |
|---------|--------|
| A | PRIMARY |
| B | SECONDARY |
| X | FIX |
| Y | RUN |
| Start | ESCAPE |
| Back | UNDO |
| Shoulders | ZOOM_IN / ZOOM_OUT |
| D-pad | SCROLL_* |
| Left stick | SCROLL_* (analog threshold) |
| Right stick Y | ZOOM_IN / ZOOM_OUT |
| Triggers L/R | UNDO / REDO |

## Windows entry point

`construo.sdl` is built with `SDL_MAIN_HANDLED` so the game keeps a normal
`main()`. MinGW builds still link `mingw32` and `SDL2::SDL2main` when
available, plus `opengl32` and common Win32 libs for the desktop GL fallback.

## Mobile / web lifecycle

`SDL2Display` handles `SDL_APP_WILLENTERBACKGROUND` by toggling `Action::RUN`
when the simulation is running (pause), and `SDL_APP_TERMINATING` /
`SDL_QUIT` as escape. The WASM shell focuses the canvas on click so
keyboard events reach SDL (`SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT`).

## Flake packaging (Pingus-aligned)

Linux hosts expose port packages via a `linuxExtras` attrset (same idea as
`pingus/flake.nix`):

| Attribute | Role |
|-----------|------|
| `arkos-sysroot` | Published ArkOS aarch64 rootfs (see `nix/r36s.nix` URL/hash) |
| `construo-r36s` | Cross-built `construo.sdl` against that sysroot |
| `construo-r36s-portmaster` | PortMaster tree under `/roms/ports` |
| `android-sdl-libs` / `construo-android` | NDK SDL2 + game native libs (`androidenv`) |
| `wasm-sdl-libs` / `construo-wasm` | Emscripten static SDL2 + game |

`checks` is `packages // appChecks` so `nix flake check` builds every package.

