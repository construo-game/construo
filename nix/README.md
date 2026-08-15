# Nix packaging for Construo ports

These files mirror the layout used by [Pingus](https://github.com/pingus/pingus)
(`nix/wasm.nix`, `nix/android.nix`, `nix/r36s.nix`). They are scaffolding for
the SDL2 + GLES2 ports and are not yet wired into `flake.nix` outputs.

| File | Target |
|------|--------|
| `wasm.nix` | Emscripten / WebAssembly |
| `android.nix` | Android APK helpers |
| `r36s.nix` | ArkOS handheld (R36S) |

Corresponding scripts and toolchains will live under `mk/wasm/`, `mk/android/`,
and `mk/r36s/` (also adapted from Pingus).

Construo no longer depends on **tinycmmc** at the top level: version and warning
helpers are inlined under `cmake/`. Helper libraries (logmich, priocpp, …) may
still pull tinycmmc through their own flakes.
