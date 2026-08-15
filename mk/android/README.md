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

