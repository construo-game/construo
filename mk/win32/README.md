# Windows (MinGW) helpers

Construo on Windows uses **SDL2 + GLES2** only (`Win32System`, no X11/GLUT).

| Item | Notes |
|------|--------|
| System | `src/win32_system.cpp` — `%APPDATA%/construo` config dir |
| Display | `SDL2Display` + `GLES2Renderer` (desktop GL fallback via `opengl32`) |
| Nix | `nix build .#construo-win64` / `.#construo-win64-bin` |
| Flake input | `SDL2-win32` (grumnix) |

## Nix cross-build (Linux host)

```bash
nix flake update   # after input changes
nix build .#construo-win64
```

## Manual MinGW

```bash
mk/win32/scripts/cross-cmake.sh
cmake --build build-win64
mk/win32/scripts/package-zip.sh build-win64/construo.sdl.exe examples /tmp/construo-win
```

`package-zip.sh` copies common MinGW runtime DLLs when found under
`MINGW_PREFIX` or next to the binary (SDL2, libstdc++, optional GLES/ANGLE).

## OpenGL / GLES on Windows

CMake always links `opengl32` on Windows targets for the compatibility
fallback in `SDL2Display`. If an ES2 driver or ANGLE is available, SDL may
still create a GLES2 context; otherwise desktop GL 2.1 is used.
