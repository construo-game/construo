# Building and installing Construo

## Native Linux (Nix)

```bash
nix develop                   # deps + tools for local cmake builds
nix build .#construo          # X11 + GLUT
nix build .#construo-sdl      # SDL2 + GLES2 (shared port path)
nix build .#construo-all      # X11 + GLUT + SDL2 together
nix flake check              # builds construo, construo-sdl, construo-all + version-smoke
# hydraJobs mirrors checks + packages for CI
```

## Native Linux (CMake)

```bash
cmake --preset linux-all
cmake --build --preset linux-all
cmake --install build/all --prefix /usr/local
```

Presets: `linux-x11-glut`, `linux-sdl2`, `linux-all`, `emscripten`.

Dependencies: CMake ≥ 3.16, C++20, glm, libsigc++-2 or 3, zlib,
and for SDL2 builds: SDL2 + GLES2/EGL (or desktop GL for the compatibility
fallback). X11/GLUT builds need libX11, freeglut, OpenGL.

## WASM (Emscripten)

1. Install EMSDK and activate `emsdk_env.sh`.
2. Build static SDL2: `SDL_SRC=… PREFIX=… mk/wasm/scripts/build-sdl2.sh`
3. Provide sigc++, helpers under the same prefix (std::format is used instead of fmt).
4. Run `mk/wasm/scripts/build-app.sh`, then `mk/wasm/scripts/serve.sh` to test:

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

## Releases

See [RELEASE.md](RELEASE.md) for versioning and tagging.
