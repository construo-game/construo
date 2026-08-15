# MinGW Win32/Win64 packaging for Construo (SDL2 + GLES2 only).
# Wired from flake.nix via pkgsCross.mingwW64 + SDL2-win32 (grumnix).
#
# Flake outputs:
#   packages.construo-win64-bin  — cross-built binary
#   packages.construo-win64      — flat tree (exe + examples)
#
{ pkgs
, SDL2-win32 ? null
}:

{
  notes = ''
    Cross-build on a Linux host:

      nix build .#construo-win64

    Manual MinGW without Nix:

      mk/win32/scripts/cross-cmake.sh
      cmake --build build-win64
      mk/win32/scripts/package-zip.sh build-win64/construo.sdl.exe examples /tmp/out

    CMake forces CONSTRUO_USE_SDL2=ON and CONSTRUO_NO_XDGCPP=ON on WIN32.
  '';

  packageZip = ../mk/win32/scripts/package-zip.sh;
  crossCmake = ../mk/win32/scripts/cross-cmake.sh;
}
