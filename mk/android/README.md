# Android packaging

Layout adapted from Pingus `mk/android/`.

| Path | Role |
|------|------|
| `app/AndroidManifest.xml` | SDLActivity, GLES2, landscape |
| `app/jni/Android.mk` | ndk-build module skeleton |
| `scripts/` | Future: build-sdl-libs / build-apk helpers |

Nix glue: `nix/android.nix`. Preferred long-term: `nix build .#construo-android`.

Touch input is handled in `SDL2Display` (finger → primary/secondary actions).

See `scripts/package-apk.sh` after SDL2 NDK libs are staged.

## Staging SDL2

```bash
export SDL_SRC=/path/to/SDL2-2.30.x
mk/android/scripts/install-sdl-libs.sh
ANDROID_NDK_HOME=… mk/android/scripts/package-apk.sh
```

The app starts fullscreen on Android (`__ANDROID__` default in `ConstruoMain`).

## Signed release (optional)

```bash
export CONSTRUO_KEYSTORE=/path/to/keystore.jks
export CONSTRUO_KEY_ALIAS=construo
export CONSTRUO_KEYSTORE_PASSWORD=…
export CONSTRUO_KEY_PASSWORD=…   # defaults to keystore password
# VERSION is read automatically; override with CONSTRUO_VERSION if needed
mk/android/scripts/stamp-version.sh
cd mk/android && gradle :app:assembleRelease
```

Without a keystore, `assembleRelease` still builds an unsigned release APK.

## Examples on device

`package-apk.sh` stages `examples/` into APK assets. Directory listing still
needs a real filesystem path, so either:

```bash
mk/android/scripts/push-examples.sh
```

or copy constructions under the app external files dir. Construo also searches
`SDL_AndroidGetInternalStoragePath()` / `ExternalStoragePath()`.

