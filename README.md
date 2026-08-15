Construo
========

Construo is a program for constructing wire-frame objects and letting
them react to physical forces. It is neither a full simulation nor a
finished game—just a small interactive toy. See `TODO` / `TODO.md` for
ideas and port status.

* Homepage: https://construo.github.io/
* Source: https://github.com/construo-game/construo

Contact: Ingo Ruhnke \<grumbel@gmail.com\>


Compilation
-----------

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Backends

| CMake option | Default | Binary | Notes |
|--------------|---------|--------|-------|
| `CONSTRUO_USE_X11` | ON | `construo.x11` | Software rendering (Xlib) |
| `CONSTRUO_USE_GLUT` | ON | `construo.glut` | Desktop OpenGL + freeglut |
| `CONSTRUO_USE_SDL2` | OFF | `construo.sdl` | SDL2 + OpenGL ES 2.0 (ports) |

SDL2 example:

```bash
# or: cmake --preset linux-sdl2 && cmake --build --preset linux-sdl2
cmake -DCONSTRUO_USE_SDL2=ON -DCONSTRUO_USE_X11=OFF -DCONSTRUO_USE_GLUT=OFF ..
cmake --build .
./construo.sdl
```

### Nix

```bash
nix flake check            # builds construo, construo-sdl, construo-all
nix build .#construo       # X11 + GLUT
nix build .#construo-sdl   # SDL2 + GLES2 (desktop validation)
nix build .#construo-all   # X11 + GLUT + SDL2
```

WASM and Win64 flake packages are wired; Android / R36S still scaffolding.
See `PORTING.md`, `TODO.md`, `nix/`, and `mk/` (layout adapted from Pingus).


Running
-------

```bash
build/construo.glut
build/construo.x11
build/construo.sdl    # when built with CONSTRUO_USE_SDL2
```

Example constructions live in `examples/`.


Agent / contributor notes
-------------------------

See `AGENTS.md` for architecture, porting rules, and the git-bundle workflow.

Status of ports
---------------

| Target | Status |
|--------|--------|
| Linux X11 / GLUT | Supported (`nix build .#construo`) |
| Linux SDL2 + GLES2 | Supported (`nix build .#construo-sdl`) |
| WASM | `nix build .#construo-wasm` (needs static helper libs for full link) |
| Win64 (MinGW) | `nix build .#construo-win64` (SDL2-win32 + pkgsCross) |
| Android / R36S | Scaffolding in `nix/` + `mk/` |

See `PORTING.md`, `INSTALL.md`, and `TODO.md`.
