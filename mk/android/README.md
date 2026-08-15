# Android packaging

Layout adapted from Pingus `mk/android/`.

| Path | Role |
|------|------|
| `app/AndroidManifest.xml` | SDLActivity, GLES2, landscape |
| `app/jni/Android.mk` | ndk-build module skeleton |
| `scripts/` | Future: build-sdl-libs / build-apk helpers |

Nix glue: `nix/android.nix`. Preferred long-term: `nix build .#construo-android`.

Touch input is handled in `SDL2Display` (finger → primary/secondary actions).
