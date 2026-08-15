# MinGW Win32/Win64 packaging scaffolding for Construo.
# Pattern: pingus flake (SDL2-win32 from grumnix) + pkgsCross.mingwW64 / mingw32.
{ pkgs }:
{
  notes = ''
    Next steps for .#construo-win64 / .#construo-win32:
    1. Add flake input (when wiring the package): git+https://github.com/grumnix/SDL2-win32.git
    2. Cross-build with pkgs.pkgsCross.mingwW64 / mingw32
    3. CMake: CONSTRUO_USE_SDL2=ON, CONSTRUO_NO_XDGCPP=ON (auto on WIN32)
    4. Ship construo.sdl.exe + examples/ in a flat zip (see Pingus mkWinFlat)
  '';
}
