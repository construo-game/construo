# Nix packaging for Construo ports

These files mirror the layout used by [Pingus](https://github.com/pingus/pingus)
(`nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix`). `wasm.nix` is wired into
`flake.nix` as `packages.construo-wasm` (static SDL2/zlib/sigc++/glm); full
link still needs static wasm helper libraries. Other targets remain stubs.

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
