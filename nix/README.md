# Nix packaging for Construo ports

These files mirror the layout used by [Pingus](https://github.com/pingus/pingus).
Helper libraries are vendored under `external/` and built by the flake via
`callPackage` (see `mkLibs` in `flake.nix`).

| Output | Status |
|--------|--------|
| `construo-wasm` | Derivation present; needs static wasm helpers for full link |
| `construo-win64` | Cross derivation + flat package via `SDL2-win32` |
| `construo-android` / `construo-r36s` | Stubs |

| File | Target |
|------|--------|
| `wasm.nix` | Emscripten / WebAssembly |
| `android.nix` | Android APK helpers |
| `r36s.nix` | ArkOS handheld (R36S) |
| `win32.nix` | MinGW Win32/Win64 (SDL2-win32) |

Corresponding scripts and toolchains will live under `mk/wasm/`, `mk/android/`,
and `mk/r36s/` (also adapted from Pingus).

Construo no longer depends on **tinycmmc** at the top level: version and warning
helpers are inlined under `cmake/`. Helper libraries (logmich, priocpp, …) may
still pull tinycmmc through their own flakes.
