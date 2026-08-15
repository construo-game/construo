# R36S / ArkOS packaging scaffolding for Construo.
{ pkgs }:
{
  toolchains = {
    aarch64 = ../mk/r36s/toolchain-arkos-aarch64.cmake;
    armhf = ../mk/r36s/toolchain-arkos-armhf.cmake;
  };

  notes = ''
    Cross-build with ARKOS_SYSROOT set and the aarch64 toolchain file.
    Prefer CONSTRUO_USE_SDL2=ON and CONSTRUO_NO_XDGCPP=ON (forced by toolchain).
    Ship construo.sdl + examples/ onto the device; match glibc via sysroot.
  '';
}
