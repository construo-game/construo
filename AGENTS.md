# AGENTS.md — Construo

## Project Overview

**Construo** is a wire-frame construction toy: place particles and springs,
watch them react to gravity and collisions. It is neither a full physics
simulation nor a finished game—just a small interactive playground written
in C++ with CMake and a Nix flake for reproducible builds.

| Path | Role |
|------|------|
| `src/` | Core simulation, GUI, tools, display backends |
| `examples/` | Sample constructions (`.construo` files) |
| `external/` | Vendored helper libraries (geomcpp, logmich, priocpp, sexpcpp, xdgcpp; tinycmmc only for subprojects) |
| `cmake/` | Inlined CMake helpers (formerly tinycmmc: version, warnings) |
| `test/` | Tests (when enabled) |
| `contrib/` | Extra / contributed material |

License: **GPL-3.0-or-later**. Prefer REUSE-style SPDX headers on new or
substantially modified source files:

```cpp
// SPDX-FileCopyrightText: 1999–2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later
```

(Adjust year/author when contributing new original work. Existing files may
use older copyright notices; do not mass-rewrite them without reason.)

Repository: https://github.com/construo-game/construo  
Homepage: https://construo.github.io/

Port / flake progress checklist: **`TODO.md`** (keep it current).

---


---

## Version numbers

- **Single source of truth:** top-level `VERSION` (e.g. `0.2.3-dev` in git).
- In git, the version always has a `-dev` suffix. Development builds expand to
  `0.2.3-dev.<revCount>+g<shortHash>[-dirty]` via CMake and/or the Nix flake.
- Release: strip `-dev`, commit `VERSION` as `0.2.3`, tag `v0.2.3`.
- CMake exposes `PROJECT_VERSION_FULL` and defines `CONSTRUO_VERSION` (and
  alias `VERSION`) for `--version` and the on-screen label.
- Packaging may pass `-DPROJECT_VERSION_FULL=...` (flake does this).

## Goals

1. Keep Construo a **fun, lightweight construction toy** that builds and runs
   cleanly on modern systems.
2. Prefer **clean design over hacks**. Document workarounds. Prefer debug
   logging when stuck.
3. Maintain **reproducible builds** via the Nix flake while remaining friendly
   to distro packaging (system libraries when appropriate).
4. Support **native Linux** (existing X11 + GLUT/OpenGL), plus new ports:
   **SDL2 + GLES2** for WASM, Android, Win32, and R36S.
5. Keep the dependency surface manageable. Helper libraries live under
   `external/` as git submodules / subtrees.

---

## Architecture Snapshot

| Area | Location / notes |
|------|------------------|
| Simulation (particles, springs, colliders, world) | `src/particle*.cpp`, `src/spring*.cpp`, `src/world*.cpp`, `src/collider*.cpp` |
| GUI / tools / file manager | `src/gui_*.cpp`, `src/worldview_*.cpp`, `src/screen_manager.cpp` |
| Graphic abstraction | `src/graphic_context.hpp`, `src/root_graphic_context.hpp`, `src/zoom_graphic_context.*` |
| Display backends (current) | `src/glut_display.*` (OpenGL + GLUT), `src/x11_display.*` (Xlib software) |
| Display backends (new) | `src/sdl2_display.*` + `src/gles2_renderer.*` (SDL2 + GLES2) |
| System / input | `src/system_context.hpp`, `src/unix_system.*`, `src/input_context.*` |
| Main entry | `src/main.cpp`, `src/construo_main.*` |
| CMake | Top-level `CMakeLists.txt` + inlined modules under `cmake/` |
| Nix flake | `flake.nix` (currently Linux-focused; extend for ports) |

Existing Linux targets (`construo.glut`, `construo.x11`) **must stay**. New
ports are **additive**.

---

## Porting strategy (WASM, Android, Win32, R36S)

Reference implementation: **https://github.com/pingus/pingus** (same author).
Copy and adapt what is useful:

- `nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix`
- `mk/wasm/`, `mk/android/`, `mk/r36s/`
- SDL2 / GLES2 patterns, Emscripten packaging, MinGW cross builds, ArkOS toolchains

### Required work (high level)

1. **SDL2 backend** — new display/input implementation that implements
   `GraphicContext` / `RootGraphicContext` (and system context) on top of SDL2.
2. **GLES2 renderer** — implement the drawing primitives (`draw_line`,
   `draw_circle`, `draw_string`, fill rects, clipping, …) with OpenGL ES 2.0
   shaders so the same code path works on WASM, Android, and embedded Linux
   (R36S). Desktop GL / X11 / GLUT remain unchanged.
3. **Nix flake outputs** for:
   - `.#construo` / `.#construo-glut` / `.#construo-x11` (existing)
   - `.#construo-gles2` (desktop validation of the GLES2 path)
   - `.#construo-wasm`
   - `.#construo-android` (or APK packaging helpers)
   - `.#construo-win32` / `.#construo-win64`
   - `.#construo-r36s` (ArkOS aarch64 / armhf)
4. **No hacks** — keep platform-specific code behind clean `#ifdef`s or
   separate translation units; prefer proper CMake options
   (`CONSTRUO_USE_SDL2`, `CONSTRUO_USE_GLES2`, …).

---

## Coding standards

- C++17 or later as required by dependencies; prefer modern C++ where it
  improves clarity.
- Match existing style in a given file (indentation, naming, includes).
- New public headers: include guards or `#pragma once` consistent with the
  rest of the tree.
- Avoid introducing heavy new dependencies without a clear need.
- Logging via `logmich`; do not spam stdout in release paths.

---

## Build & packaging rules

- Primary build system: **CMake**.
- Reproducible packaging: **Nix flake** (`flake.nix`).
- Prefer `find_package` / pkg-config so distro builds can use system libraries;
  fall back to `external/` when needed (tinycmmc pattern).
- Do not hard-require exact flake pins for every downstream build.
- Missing dependencies are packaging bugs: fail clearly at configure time.

Helper libraries live under `external/`. CMake resolves them via
`find_package` then `add_subdirectory(external/…)`.

**tinycmmc:** not used by construo itself anymore — helpers are inlined under
`cmake/`. Library flakes may still depend on tinycmmc independently.

---

## Git workflow (for agents / contributors)

Repository: https://github.com/construo-game/construo.git

- Code is handed over as **git bundles**. Bundles must:
  - stack cleanly on top of the previous one;
  - be continuously numbered (`construo-001-…`, `construo-002-…`, …);
  - never reuse a number, even after resets;
  - use `HEAD` as the ref.
- Make every coherent change a **separate, small, task-focused commit**.
- Author for all commits:

  ```
  Ingo Ruhnke <grumbel@gmail.com>
  ```

  with trailer:

  ```
  Co-authored-by: Grok <grok@x.ai>
  ```

- After each coherent series, leave a **detailed suggested commit message**
  (subject ≤ ~72 chars, body explaining why and what) when producing bundles.
- Update documentation (`README.md`, `NEWS`, `TODO.md`, …) in the same series
  when user-visible behaviour or build requirements change.

### History is append-only (agents)

- **Never rewrite, reset, rebase, or re-root history** against `origin/master`
  or an earlier tip unless the human **explicitly** asks for a rollback.
- **Never** `git reset --hard` to `origin`, re-clone over a lost tip and
  “rebuild” prior commits with new SHAs, or ship a bundle whose parent is an
  older commit when the consumer already applied a newer tip.
- If the agent workspace lost local objects, **recover the consumer tip first**
  and continue from that tip. Do not invent a parallel history from an older base.
- Fix mistakes with **new commits** (or `git revert`) on top of the current tip.

---

## What not to do

- Do not remove or break the existing X11 and GLUT/OpenGL backends.
- Do not introduce platform hacks that bypass the `GraphicContext` abstraction.
- Do not bulk-reformat unrelated code in the same commit as a functional change.
- Do not commit generated or temporary files (build dirs, editor autosaves, …).

---

## Reference

When in doubt about WASM / Android / Win32 / R36S packaging, examine the
corresponding files in the Pingus repository and adapt them carefully to
Construo’s smaller surface (no sound stack required initially, simpler data
layout under `examples/`).
