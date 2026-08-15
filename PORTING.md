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

Scaffolding and shared runtime code are in tree. Flake outputs for wasm,
Android APK, MinGW zip, and device packages still need prebuilt SDL2 prefixes
and cross-built helper libraries (fmt, sigc++, geom, logmich, prio). Tracked in
`TODO.md`.

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
