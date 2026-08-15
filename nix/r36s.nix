# R36S / ArkOS packaging for Construo (SDL2 + GLES2).
# Cross-compile requires ARKOS_SYSROOT matching device glibc.
#
{ pkgs }:

{
  toolchains = {
    aarch64 = ../mk/r36s/toolchain-arkos-aarch64.cmake;
    armhf = ../mk/r36s/toolchain-arkos-armhf.cmake;
  };

  packagePort = ../mk/r36s/scripts/package-port.sh;

  notes = ''
    Cross-build:

      export ARKOS_SYSROOT=/path/to/aarch64-rootfs
      cmake -S . -B build-r36s \
        -DCMAKE_TOOLCHAIN_FILE=mk/r36s/toolchain-arkos-aarch64.cmake
      cmake --build build-r36s

    Package for device:

      mk/r36s/scripts/package-port.sh build-r36s/construo.sdl examples /tmp/construo-port

    Launcher uses -f -g 640x480 (native panel). See controls.txt in the
    package and CROSSCOMPILE.md for sysroot notes.
  '';
}
