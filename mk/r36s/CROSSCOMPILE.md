# Cross-compiling Construo for R36S (ArkOS)

**Target:** R36S handheld — Rockchip **RK3326** (4× Cortex-A35), **640×480**, Mali-G31.  
**Firmware:** ArkOS / community images (Ubuntu 19.10–era userspace; old glibc).

| Setting | Value |
|---------|--------|
| Video | **SDL2** + **GLES2** (`CONSTRUO_USE_SDL2=ON`) |
| X11 / GLUT | off |
| XDG | off (`CONSTRUO_NO_XDGCPP=ON`) |
| Preferred ABI | **aarch64-linux-gnu** |
| Optional ABI | **arm-linux-gnueabihf** |

Native resolution 800×600 defaults may be oversized; pass smaller screen size via settings or start fullscreen and scale.

## Sysroot requirement

Stock ArkOS uses older glibc. Linking against modern nixpkgs/Ubuntu 22.04+ glibc produces binaries that fail on device (`GLIBC_2.3x not found`). Use a matching sysroot (device copy, eoan/buster chroot).

```bash
export ARKOS_SYSROOT=/path/to/aarch64-rootfs
cmake -S . -B build-r36s \
  -DCMAKE_TOOLCHAIN_FILE=mk/r36s/toolchain-arkos-aarch64.cmake
cmake --build build-r36s
```

## Packaging on device

Install `construo.sdl` plus the `examples/` tree (e.g. under `/roms/ports/construo/` or similar). Launch with working directory set so relative paths resolve, or set datadir accordingly.

See also Pingus `mk/r36s/` for sysroot extraction scripts if needed later.

## Debug symbols (R36S nix package)

`nix/r36s.nix` builds with `CMAKE_BUILD_TYPE=RelWithDebInfo`, `-g`,
`-fno-omit-frame-pointer`, and `dontStrip = true` so the device binary
keeps symbols for `gdb` / `eu-addr2line` on SIGBUS and similar crashes.

## Launcher defaults

`mk/r36s/scripts/package-port.sh` writes `construo.sh` that starts with:

```bash
./construo.sdl -f -g 640x480
```

Override geometry on the command line if needed. Controller mapping is
documented in `controls.txt` inside the package.

