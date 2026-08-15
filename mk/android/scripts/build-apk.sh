#!/usr/bin/env bash
# Build Construo native libs (and optional Gradle APK) using the Pingus-style
# jni layout:
#   jni/Android.mk          — all-subdir-makefiles
#   jni/Application.mk
#   jni/SDL/Android.mk      — PREBUILT_SHARED_LIBRARY SDL2 (required)
#   jni/SDL/include/…       — SDL headers
#   jni/src/Android.mk      — libmain
#   jni/src/src + external  — staged sources (optional; Android.mk can fall
#                             back to the monorepo root)
#
# Required: ANDROID_HOME or ANDROID_NDK_HOME
# Optional: SDL_ANDROID_LIBS (dir with lib/<abi>/libSDL2.so + include/)
#           CONSTRUO_VERSION
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
APP="$ROOT/mk/android/app"

resolve_ndk() {
  if [ -n "${ANDROID_NDK_HOME:-}" ] && [ -x "${ANDROID_NDK_HOME}/ndk-build" ]; then
    printf '%s' "$ANDROID_NDK_HOME"; return
  fi
  if [ -z "${ANDROID_HOME:-}" ]; then
    echo "error: set ANDROID_HOME or ANDROID_NDK_HOME" >&2; exit 1
  fi
  if [ -x "$ANDROID_HOME/ndk-bundle/ndk-build" ]; then
    printf '%s' "$ANDROID_HOME/ndk-bundle"; return
  fi
  for d in "$ANDROID_HOME/ndk"/*; do
    [ -x "$d/ndk-build" ] || continue
    printf '%s' "$d"; return
  done
  echo "error: no ndk-build found" >&2; exit 1
}

NDK="$(resolve_ndk)"
echo "==> NDK=$NDK"

# If SDL prebuilt module is missing, try to stage from SDL_ANDROID_LIBS.
if [ ! -f "$APP/jni/SDL/Android.mk" ]; then
  if [ -n "${SDL_ANDROID_LIBS:-}" ] && [ -d "$SDL_ANDROID_LIBS" ]; then
    echo "==> Staging SDL2 prebuilt from $SDL_ANDROID_LIBS"
    mkdir -p "$APP/jni/SDL/include"
    cat > "$APP/jni/SDL/Android.mk" <<MK
LOCAL_PATH := \$(call my-dir)
include \$(CLEAR_VARS)
LOCAL_MODULE := SDL2
LOCAL_SRC_FILES := ${SDL_ANDROID_LIBS}/lib/\$(TARGET_ARCH_ABI)/libSDL2.so
LOCAL_EXPORT_C_INCLUDES := \$(LOCAL_PATH)/include \$(LOCAL_PATH)/include/SDL2
include \$(PREBUILT_SHARED_LIBRARY)
MK
    if [ -d "$SDL_ANDROID_LIBS/include" ]; then
      cp -a "$SDL_ANDROID_LIBS/include"/. "$APP/jni/SDL/include/"
    fi
  else
    echo "error: missing $APP/jni/SDL/Android.mk" >&2
    echo "       Build android-sdl-libs (nix build .#android-sdl-libs) or set" >&2
    echo "       SDL_ANDROID_LIBS to a tree with lib/<abi>/libSDL2.so and include/" >&2
    exit 1
  fi
fi

echo "==> Building native libs (libmain + prebuilt SDL2)"
"$NDK/ndk-build" -C "$APP" "$@"

if command -v gradle >/dev/null 2>&1 && [ -f "$ROOT/mk/android/app/build.gradle" ]; then
  echo "==> Gradle assemble (if SDK configured)"
  (cd "$ROOT/mk/android" && gradle :app:assembleDebug) || \
    echo "NOTE: gradle assemble skipped/failed; native libs are under $APP/libs/"
else
  echo "Native libs under $APP/libs/ — package with your preferred APK tooling."
fi
