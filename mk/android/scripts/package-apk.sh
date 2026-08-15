#!/usr/bin/env bash
# Stage Construo sources next to the Android.mk tree and invoke ndk-build.
# Requires: ANDROID_NDK_HOME, SDL2 Android project under mk/android/app/jni/SDL
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/../../.." && pwd)"
APP="$ROOT/mk/android/app"
JNI="$APP/jni"

"$ROOT/mk/android/scripts/stamp-version.sh"
export CONSTRUO_VERSION="$(tr -d '
' < "$ROOT/VERSION")"

echo "Construo Android packaging helper"
echo "  ROOT=$ROOT"
echo "  Ensure SDL2 is installed under $JNI (see install-sdl-libs.sh from Pingus)."
echo "  ANDROID_NDK_HOME=${ANDROID_NDK_HOME:-<unset>}"

if [[ -z "${ANDROID_NDK_HOME:-}" ]]; then
  echo "Set ANDROID_NDK_HOME to the NDK root." >&2
  exit 1
fi

# Stage example constructions into APK assets (readable via asset manager / extracts).
ASSETS="$APP/src/main/assets"
mkdir -p "$ASSETS"
rm -rf "$ASSETS/examples"
cp -a "$ROOT/examples" "$ASSETS/examples"
echo "Staged examples → $ASSETS/examples"

# Point Android.mk at the real source tree via absolute path already in Android.mk
"$ANDROID_NDK_HOME/ndk-build" -C "$APP" "$@"

echo "Native libs under $APP/libs/ — package with gradle/ant using AndroidManifest.xml"
