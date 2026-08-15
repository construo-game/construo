# Android packaging for Construo (SDL2 + GLES2), following pingus/nix/android.nix.
#
# Usage from flake:
#   android = import ./nix/android.nix {
#     inherit pkgs androidSdk buildToolsVersion packagePlatform compilePlatform targetAbis;
#     sdlSrc = sdl2-src;
#     sdlVersion = "2.30.3";
#   };
#   android.mkApk { … }
{ pkgs
, sdlSrc
, sdlVersion ? "2.30.3"
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
      export SDL_SRC="${sdlSrc}"
      export TARGET_ABIS="${targetAbisStr}"
      export PACKAGE_PLATFORM="${packagePlatform}"
      export OUT_DIR="$PWD/sdl-out"
      mkdir -p "$OUT_DIR"
      # Reuse project helper when present; otherwise ndk-build SDL2 jni tree.
      if [ -f ${../mk/android/scripts/install-sdl-libs.sh} ]; then
        # install-sdl-libs.sh expects to write into the app tree; stage a temp app.
        mkdir -p work/app/jni
        cp -a ${../mk/android/app}/jni/Application.mk work/app/jni/ 2>/dev/null || cp ${applicationMk} work/app/jni/Application.mk
        # Minimal install: build SDL2 Android.mk from SDL_SRC
        mkdir -p work/app/jni/SDL
        cp -a "${sdlSrc}"/. work/app/jni/SDL/
        printf '%s\n' 'include $(call all-subdir-makefiles)' > work/app/jni/Android.mk
        cp ${applicationMk} work/app/jni/Application.mk
        ndk-build -C work/app -j''${NIX_BUILD_CORES:-4} NDK_LIBS_OUT="$OUT_DIR/lib" NDK_OUT="$PWD/obj"
      fi
      runHook postBuild
    '';
    installPhase = ''
      runHook preInstall
      mkdir -p $out
      if [ -d sdl-out ]; then cp -a sdl-out/. $out/; fi
      if [ -d work/app/libs ]; then mkdir -p $out/lib; cp -a work/app/libs/. $out/lib/; fi
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

        mkdir -p work
        cp -a ${appDir}/. work/app/
        chmod -R u+w work/app

        # Stage SDL prebuilts
        mkdir -p work/app/jni/SDL
        if [ -d ${sdlAndroidLibs}/lib ]; then
          mkdir -p work/app/libs
          cp -a ${sdlAndroidLibs}/lib/. work/app/libs/ || true
        fi

        # Point Android.mk at sources
        export CONSTRUO_SRC_ROOT="$(pwd)/work/srcroot"
        mkdir -p "$CONSTRUO_SRC_ROOT"
        cp -a ${gameSrcDir} "$CONSTRUO_SRC_ROOT/src"
        cp -a ${gameExternalDir} "$CONSTRUO_SRC_ROOT/external"
        # Patch Android.mk CONSTRUO_SRC_ROOT if relative
        sed -i "s|CONSTRUO_SRC_ROOT := .*|CONSTRUO_SRC_ROOT := $CONSTRUO_SRC_ROOT|" work/app/jni/Android.mk || true

        if [ -n "${toString glmIncludeDir}" ] && [ -d "${glmIncludeDir}" ]; then
          export GLM_ROOT="${glmIncludeDir}"
        fi

        # Examples as assets
        mkdir -p work/app/src/main/assets
        if [ -d ${gameExamplesDir} ]; then
          cp -a ${gameExamplesDir} work/app/src/main/assets/examples
        fi

        ndk-build -C work/app -j''${NIX_BUILD_CORES:-4}

        # Prefer existing package scripts when Gradle is not fully wired.
        if command -v aapt >/dev/null 2>&1; then
          echo "ndk-build finished; APK packaging via aapt is optional in this phase"
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
  inherit sdlAndroidLibs mkApk applicationMk topAndroidMk;
}
