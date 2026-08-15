# Top-level jni/Android.mk — recurse into SDL/ (prebuilt) and src/ (libmain).
# Matches the Pingus layout used by nix/android.nix mkApk staging.
include $(call all-subdir-makefiles)
