# Android packaging for Construo (SDL2 + GLES2), following pingus/nix/android.nix.
#
# Usage from flake:
#   android = import ./nix/android.nix {
#     inherit pkgs androidSdk buildToolsVersion packagePlatform compilePlatform targetAbis;
#     sdlSrc = sdl2-src;
#     sdlVersion = "2.32.8";
#   };
#   android.mkApk { … }
{ pkgs
, sdlSrc
, sdlVersion ? "2.32.8"
, androidSdk
, buildToolsVersion
, packagePlatform
, compilePlatform
, targetAbis
}:

let
  lib = pkgs.lib;
  targetAbisStr = lib.concatStringsSep " " targetAbis;

  topAndroidMk = pkgs.writeTextFile {
    name = "Android.mk";
    text = "include $(call all-subdir-makefiles)\n";
  };

  applicationMk = pkgs.writeTextFile {
    name = "Application.mk";
    text = ''
      APP_STL := c++_shared
      APP_ABI := ${targetAbisStr}
      APP_PLATFORM := android-${packagePlatform}
      APP_CPPFLAGS := -std=c++20 -frtti -fexceptions
      APP_MODULES := main
    '';
  };

  # Prebuilt SDL2 shared libs per ABI (ndk-build of SDL2 sources).
  sdlAndroidLibs = pkgs.stdenvNoCC.mkDerivation {
    pname = "sdl2-android-libs";
    version = sdlVersion;
    dontUnpack = true;
    nativeBuildInputs = [ androidSdk pkgs.jdk17 pkgs.gnumake ];
    env = {
      BUILD_TOOLS_VERSION = buildToolsVersion;
      PACKAGE_PLATFORM = packagePlatform;
      TARGET_ABIS = targetAbisStr;
    };
    buildPhase = ''
      runHook preBuild
      export ANDROID_SDK_ROOT="${androidSdk}/libexec/android-sdk"
      export ANDROID_NDK_HOME="$(echo "$ANDROID_SDK_ROOT"/ndk/* | awk '{print $1}')"
      export ANDROID_HOME="$ANDROID_SDK_ROOT"
      export PATH="$ANDROID_NDK_HOME:$PATH"
      export OUT_DIR="$PWD/sdl-out"
      mkdir -p "$OUT_DIR"

      # Writable staging (nix store copies are read-only).
      # Stage only as jni/SDL2 so all-subdir-makefiles defines the module once.
      mkdir -p work/app/jni/SDL2
      cp -a "${sdlSrc}"/. work/app/jni/SDL2/
      chmod -R u+w work
      printf '%s\n' 'include $(call all-subdir-makefiles)' > work/app/jni/Android.mk
      cat > work/app/jni/Application.mk <<MK
APP_STL := c++_shared
APP_PLATFORM := android-${packagePlatform}
APP_MODULES := SDL2
MK
      chmod -R u+w work

      export NDK_PROJECT_PATH="$PWD/work/app"
      # One ABI at a time: full SDL + multi-ABI + high -j often OOMs the sandbox
      # and only leaves truncated compile lines in the log.
      for abi in ${targetAbisStr}; do
        echo "==> ndk-build SDL2 ABI=$abi"
        ndk-build -C work/app -j2 \
          APP_ABI="$abi" \
          NDK_LIBS_OUT="$OUT_DIR/lib" \
          NDK_OUT="$PWD/obj-$abi" \
          NDK_DEBUG=0
      done

      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out/lib $out/include
      if [ -d sdl-out/lib ]; then cp -a sdl-out/lib/. $out/lib/; fi
      if [ -d work/app/libs ]; then cp -a work/app/libs/. $out/lib/; fi
      # Headers for the app's ndk-build tree (jni/SDL/include).
      if [ -d work/app/jni/SDL2/include ]; then
        cp -a work/app/jni/SDL2/include/. $out/include/
      elif [ -d "${sdlSrc}/include" ]; then
        cp -a "${sdlSrc}/include"/. $out/include/
      fi
      # Also keep Java glue from SDL for packaging scripts.
      if [ -d "${sdlSrc}/android-project/app/src/main/java" ]; then
        mkdir -p $out/java
        cp -a "${sdlSrc}/android-project/app/src/main/java"/. $out/java/
      fi
      runHook postInstall
    '';
    meta = with lib; {
      description = "SDL2 shared libraries for Construo Android";
      license = licenses.zlib;
      platforms = platforms.linux;
      hydraPlatforms = [ ];
    };
  };

  # Prebuilt SDL2 for the app's ndk-build tree (sibling of jni/src/).
  # LOCAL_SRC_FILES path is absolute into the Nix store so ndk-build does not
  # need the .so copied under jni/ — same pattern as Pingus.
  sdlPrebuiltAndroidMk = pkgs.writeTextFile {
    name = "SDL2-prebuilt-Android.mk";
    text = ''
      LOCAL_PATH := $(call my-dir)
      include $(CLEAR_VARS)
      LOCAL_MODULE := SDL2
      LOCAL_SRC_FILES := ${sdlAndroidLibs}/lib/$(TARGET_ARCH_ABI)/libSDL2.so
      LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include $(LOCAL_PATH)/include/SDL2
      include $(PREBUILT_SHARED_LIBRARY)
    '';
  };

  mkApk = {
    appName ? "construo"
  , appDir ? ../mk/android/app
  , outApkName ? "construo.apk"
  , keystore ? null
  , gameSrcDir ? ../src
  , gameExternalDir ? ../external
  , glmIncludeDir ? null
  , gameExamplesDir ? ../examples
  , gameVersion ? "0.2.3-dev"
  }:
    pkgs.stdenvNoCC.mkDerivation {
      pname = appName;
      version = gameVersion;
      dontUnpack = true;
      nativeBuildInputs = [ androidSdk pkgs.jdk17 pkgs.zip pkgs.gnumake pkgs.unzip ];
      env = {
        BUILD_TOOLS_VERSION = buildToolsVersion;
        PACKAGE_PLATFORM = packagePlatform;
        COMPILE_PLATFORM = compilePlatform;
        APP_NAME = appName;
        CONSTRUO_VERSION = gameVersion;
      };
      buildPhase = ''
        runHook preBuild
        export ANDROID_SDK_ROOT="${androidSdk}/libexec/android-sdk"
        export ANDROID_NDK_HOME="$(echo "$ANDROID_SDK_ROOT"/ndk/* | awk '{print $1}')"
        export ANDROID_HOME="$ANDROID_SDK_ROOT"
        export PATH="$ANDROID_NDK_HOME:$ANDROID_SDK_ROOT/build-tools/${buildToolsVersion}:$PATH"

        mkdir -p work/app/jni/src work/app/jni/SDL

        # Manifest / res from appDir
        cp -a ${appDir}/AndroidManifest.xml work/app/ 2>/dev/null || true
        if [ -d ${appDir}/res ]; then cp -a ${appDir}/res work/app/; fi
        if [ -f ${appDir}/build.gradle ]; then cp -a ${appDir}/build.gradle work/app/; fi

        # Top-level jni makefiles (all-subdir + Application.mk)
        cp ${topAndroidMk} work/app/jni/Android.mk
        cp ${applicationMk} work/app/jni/Application.mk

        # Game module Android.mk under jni/src/
        cp ${appDir}/jni/src/Android.mk work/app/jni/src/Android.mk

        # Stage game sources + external helpers under jni/src/ so
        # CONSTRUO_SRC_ROOT := $(LOCAL_PATH) finds src/main.cpp.
        cp -a ${gameSrcDir} work/app/jni/src/src
        cp -a ${gameExternalDir} work/app/jni/src/external
        # VERSION for CONSTRUO_VERSION fallback in Android.mk
        if [ -f ${gameSrcDir}/../VERSION ]; then
          cp ${gameSrcDir}/../VERSION work/app/jni/src/VERSION 2>/dev/null || true
        fi
        # Write version file explicitly for reproducible builds
        echo -n "${gameVersion}" > work/app/jni/src/VERSION
        # String macros via header (ndk-build strips quotes from -DFOO="bar")
        cat > work/app/jni/src/construo_android_config.h <<CFG
#pragma once
#define CONSTRUO_VERSION "${gameVersion}"
#define VERSION CONSTRUO_VERSION
#define PACKAGE "construo"
#define PACKAGE_STRING "construo ${gameVersion}"
#define CONSTRUO_DATADIR "/"
CFG
        chmod -R u+w work/app/jni/src

        # SDL2 prebuilt module + headers
        cp ${sdlPrebuiltAndroidMk} work/app/jni/SDL/Android.mk
        mkdir -p work/app/jni/SDL/include
        if [ -d ${sdlAndroidLibs}/include ]; then
          cp -a ${sdlAndroidLibs}/include/. work/app/jni/SDL/include/
        fi
        chmod -R u+w work/app/jni/SDL

        # Place .so under libs/ for packaging. Must be writable: ndk-build
        # clean-installed-binaries tries to rm them (nix store files are 0444).
        if [ -d ${sdlAndroidLibs}/lib ]; then
          mkdir -p work/app/libs
          cp -a ${sdlAndroidLibs}/lib/. work/app/libs/ || true
          chmod -R u+w work/app/libs
        fi
        # Whole app tree writable for ndk-build intermediate outputs.
        chmod -R u+w work/app

        if [ -n "${toString glmIncludeDir}" ] && [ -d "${glmIncludeDir}" ]; then
          # Ensure glm is visible; prefer GLM_ROOT if Android.mk uses it.
          export GLM_ROOT="${glmIncludeDir}"
          # Also stage under external if missing
          if [ ! -d work/app/jni/src/external/glm ] && [ -d "${glmIncludeDir}/glm" ]; then
            mkdir -p work/app/jni/src/external/glm
            cp -a "${glmIncludeDir}/glm"/. work/app/jni/src/external/glm/ || \
              cp -a "${glmIncludeDir}"/. work/app/jni/src/external/ || true
            chmod -R u+w work/app/jni/src/external
          fi
        fi

        # Examples as assets
        mkdir -p work/app/src/main/assets
        if [ -d ${gameExamplesDir} ]; then
          cp -a ${gameExamplesDir} work/app/src/main/assets/examples
        fi

        echo "==> ndk-build libmain + prebuilt SDL2"
        ndk-build -C work/app -j''${NIX_BUILD_CORES:-4} NDK_DEBUG=0

        if command -v aapt >/dev/null 2>&1; then
          echo "ndk-build finished; full aapt/apksigner packaging can extend this phase"
        fi
        runHook postBuild
      '';
      installPhase = ''
        runHook preInstall
        mkdir -p $out/lib $out/share/construo-android
        if [ -d work/app/libs ]; then cp -a work/app/libs/. $out/lib/; fi
        if [ -d work/app/obj ]; then cp -a work/app/obj $out/share/construo-android/ || true; fi
        echo "${outApkName}" > $out/share/construo-android/OUT_APK_NAME.txt
        echo "Native libs built for Construo Android (${gameVersion})." > $out/README.txt
        echo "Full aapt/apksigner packaging can extend this derivation (see pingus)." >> $out/README.txt
        # List produced shared objects for verification
        find $out/lib -name '*.so' 2>/dev/null | sort >> $out/README.txt || true
        runHook postInstall
      '';
      meta = with lib; {
        description = "Construo Android native libraries (NDK)";
        license = licenses.gpl3Plus;
        platforms = platforms.linux;
        hydraPlatforms = [ ];
      };
    };

in {
  inherit sdlAndroidLibs mkApk applicationMk topAndroidMk sdlPrebuiltAndroidMk;
}
