# Windows (MinGW) helpers

Construo on Windows uses **SDL2 + GLES2** only (`Win32System`, no X11/GLUT).

| Item | Notes |
|------|--------|
| System | `src/win32_system.cpp` — `%APPDATA%/construo` config dir |
| Display | `SDL2Display` + `GLES2Renderer` |
| Nix | `nix/win32.nix`, flake input `SDL2-win32` (grumnix) |

Cross-build sketch:

```bash
# From a Linux host with nix:
# nix build .#construo-win64   # once fully wired
```

Manual MinGW:

```bash
cmake -S . -B build-win \
  -DCMAKE_TOOLCHAIN_FILE=...mingw... \
  -DCONSTRUO_USE_SDL2=ON
```
