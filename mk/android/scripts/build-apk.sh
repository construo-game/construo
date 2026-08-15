#!/usr/bin/env bash
# Build Construo APK via ndk-build + optional Gradle.
# Required: ANDROID_HOME or ANDROID_NDK_HOME, staged SDL2 under app/jni/
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
echo "==> Building native libs"
"$NDK/ndk-build" -C "$APP" "$@"

if command -v gradle >/dev/null && [ -f "$ROOT/mk/android/app/build.gradle" ]; then
  echo "==> Gradle assemble (if SDK configured)"
  (cd "$ROOT/mk/android" && gradle :app:assembleDebug) || \
    echo "NOTE: gradle assemble skipped/failed; native libs are under $APP/libs/"
else
  echo "Native libs under $APP/libs/ — package with your preferred APK tooling."
fi
