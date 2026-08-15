#!/usr/bin/env bash
# Stage SDL2 Android project sources under mk/android/app/jni/ for ndk-build.
#
# Env:
#   SDL_SRC   - path to SDL2 source tree (required), e.g. SDL2-2.30.3
#   JNI_DIR   - override jni dir (default: mk/android/app/jni relative to repo)
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
JNI="${JNI_DIR:-$ROOT/mk/android/app/jni}"

if [ -z "${SDL_SRC:-}" ]; then
  echo "error: SDL_SRC must point at an SDL2 source tree" >&2
  exit 1
fi
if [ ! -f "$SDL_SRC/Android.mk" ] && [ ! -f "$SDL_SRC/android-project/app/jni/SDL/Android.mk" ]; then
  echo "error: $SDL_SRC does not look like SDL2 sources (missing Android.mk)" >&2
  exit 1
fi

mkdir -p "$JNI"
# Official SDL2 trees ship Android.mk at the root of the library sources.
if [ -f "$SDL_SRC/Android.mk" ]; then
  rm -rf "$JNI/SDL"
  mkdir -p "$JNI/SDL"
  # ndk-build import-module looks for SDL2; keep a stable name.
  cp -a "$SDL_SRC/." "$JNI/SDL/"
  # Symlink module name expected by Android.mk LOCAL_SHARED_LIBRARIES := SDL2
  if [ ! -e "$JNI/SDL2" ]; then
    ln -sfn SDL "$JNI/SDL2"
  fi
else
  cp -a "$SDL_SRC/android-project/app/jni/SDL" "$JNI/"
  ln -sfn SDL "$JNI/SDL2"
fi

# Application.mk may already exist; do not overwrite custom ABIs.
if [ ! -f "$JNI/Application.mk" ]; then
  cat > "$JNI/Application.mk" <<'MK'
APP_ABI := armeabi-v7a arm64-v8a x86_64
APP_PLATFORM := android-22
APP_STL := c++_shared
APP_CPPFLAGS := -std=c++20 -frtti -fexceptions
MK
fi

echo "SDL2 staged under $JNI/SDL (module SDL2)"
echo "Next: ANDROID_NDK_HOME=… mk/android/scripts/package-apk.sh"
