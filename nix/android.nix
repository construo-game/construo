# Android packaging for Construo (SDL2 + GLES2 via NDK / Gradle).
#
{ pkgs }:

{
  appDir = ../mk/android/app;

  scripts = {
    installSdl = ../mk/android/scripts/install-sdl-libs.sh;
    packageApk = ../mk/android/scripts/package-apk.sh;
    buildApk = ../mk/android/scripts/build-apk.sh;
  };

  notes = ''
    1. Stage SDL2:

         SDL_SRC=/path/to/SDL2 mk/android/scripts/install-sdl-libs.sh

    2. Build native libs + optional Gradle APK:

         ANDROID_NDK_HOME=… mk/android/scripts/package-apk.sh
         # or: mk/android/scripts/build-apk.sh

    Android.mk compiles game sources plus vendored logmich/sexpcpp/priocpp
    (sexp path). Full signed release still needs a keystore and CI wiring.
  '';
}
