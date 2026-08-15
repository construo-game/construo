#!/usr/bin/env bash
# Placeholder APK build driver for Construo.
# Port the Pingus mk/android/scripts/{build-sdl-libs,build-apk}.sh flow:
#   1. Build or install SDL2 for Android ABIs
#   2. Stage sources under app/jni
#   3. ndk-build + aapt/apksigner
set -euo pipefail
echo "construo android APK build is not fully wired yet — see mk/android/README.md" >&2
exit 1
