# Android packaging scaffolding for Construo.
# Mirror pingus/nix/android.nix once SDL2 NDK libs and APK scripts are ready.
{ pkgs }:
{
  appDir = ../mk/android/app;

  notes = ''
    Next steps:
    1. Stage SDL2 Android binaries (see Pingus mk/android/scripts/build-sdl-libs.sh)
    2. Expand app/jni/Android.mk with construo + external sources
    3. Wire flake output .#construo-android to scripts/build-apk.sh
  '';
}
