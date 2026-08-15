# Android packaging

Layout adapted from Pingus `mk/android/` (SDL2 as prebuilt, game under `jni/src/`).

| Path | Role |
|------|------|
| `app/AndroidManifest.xml` | SDLActivity, GLES2, landscape |
| `app/jni/Android.mk` | `include $(call all-subdir-makefiles)` |
| `app/jni/Application.mk` | ABI / STL / `APP_MODULES := main` |
| `app/jni/src/Android.mk` | `libmain` (Construo + external sources) |
| `app/jni/SDL/` | Staged at build time: `Android.mk` (PREBUILT SDL2) + `include/` |
| `scripts/` | `build-apk.sh`, `package-apk.sh`, version stamp, push-examples |

Nix glue: `nix/android.nix`. Preferred:

```bash
nix build .#android-sdl-libs   # prebuilt libSDL2.so per ABI
nix build .#construo-android   # ndk-build libmain + staged SDL2
```

`construo-android` stages:

1. Top-level `jni/Android.mk` + `Application.mk`
2. `jni/SDL/Android.mk` as `PREBUILT_SHARED_LIBRARY` pointing at `android-sdl-libs`
3. Game sources under `jni/src/{src,external}/`
4. Runs `ndk-build` (no `import-module` / `NDK_MODULE_PATH`)

Touch input is handled in `SDL2Display` (finger → primary/secondary actions).

## Local ndk-build (without full Nix APK)

```bash
# After: nix build .#android-sdl-libs
export SDL_ANDROID_LIBS=$(nix build .#android-sdl-libs --print-out-path)
export ANDROID_NDK_HOME=…   # or ANDROID_HOME
mk/android/scripts/build-apk.sh
```

The app starts fullscreen on Android (`__ANDROID__` default in `ConstruoMain`).

## Signed release (optional)

```bash
export CONSTRUO_KEYSTORE=/path/to/keystore.jks
export CONSTRUO_KEY_ALIAS=construo
export CONSTRUO_KEYSTORE_PASSWORD=…
export CONSTRUO_KEY_PASSWORD=…   # defaults to keystore password
mk/android/scripts/stamp-version.sh
cd mk/android && gradle :app:assembleRelease
```

Without a keystore, `assembleRelease` still builds an unsigned release APK.

## Examples on device

`package-apk.sh` / Nix staging put `examples/` into APK assets. Directory
listing still needs a real filesystem path, so either:

```bash
mk/android/scripts/push-examples.sh
```

or copy constructions under the app external files dir. Construo also searches
`SDL_AndroidGetInternalStoragePath()` / `ExternalStoragePath()`.
