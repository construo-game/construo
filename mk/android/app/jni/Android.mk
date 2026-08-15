# Android.mk skeleton for Construo (SDL2 Application.mk / ndk-build path).
# Full source lists and dep staging follow the Pingus pattern once the
# NDK package is wired (see nix/android.nix and scripts/).

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := main
# Sources are expected to be staged under jni/ by the packaging script.
# LOCAL_SRC_FILES will be generated or listed explicitly in a later revision.
LOCAL_LDLIBS := -lGLESv2 -llog -landroid
include $(BUILD_SHARED_LIBRARY)
