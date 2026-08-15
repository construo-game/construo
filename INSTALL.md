# Building and installing Construo

## Native Linux (Nix)

```bash
nix build .#construo          # X11 + GLUT
nix build .#construo-sdl      # SDL2 + GLES2 (shared port path)
```

## Native Linux (CMake)

```bash
cmake --preset linux-all
cmake --build --preset linux-all
cmake --install build/all --prefix /usr/local
```

Presets: `linux-x11-glut`, `linux-sdl2`, `linux-all`, `emscripten`.

Dependencies: CMake ≥ 3.16, C++20, fmt, glm, libsigc++-2 or 3, zlib,
and for SDL2 builds: SDL2 + GLES2/EGL (or desktop GL for the compatibility
fallback). X11/GLUT builds need libX11, freeglut, OpenGL.

## WASM (Emscripten)

1. Install EMSDK and activate `emsdk_env.sh`.
2. Provide static SDL2 (and fmt, sigc++, helpers) under a prefix.
3. Run `mk/wasm/scripts/build-app.sh` or:

```bash
emcmake cmake --preset emscripten -DCMAKE_PREFIX_PATH=/path/to/sdl-wasm
cmake --build --preset emscripten
```

Serve the produced `.html` / `.js` / `.wasm` (and `.data` if examples were preloaded).

## Android

See `mk/android/`. Stage SDL2 under `jni/`, then:

```bash
export ANDROID_NDK_HOME=...
mk/android/scripts/package-apk.sh
```

## Win32 / Win64 (MinGW)

Cross-build with a MinGW toolchain and SDL2. Package with:

```bash
mk/win32/scripts/package-zip.sh /path/to/construo.sdl examples outdir
```

## R36S (ArkOS)

Cross-compile against the device sysroot (`mk/r36s/` toolchains), then:

```bash
mk/r36s/scripts/package-port.sh /path/to/construo.sdl examples outdir
```

Gamepad input is supported via SDL2 game controllers (D-pad, A/B/X/Y, shoulders).
