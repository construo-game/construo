# Android.mk for Construo libmain (SDL2 + GLES2).
# Expected layout after staging (see nix/android.nix / Pingus build-apk.sh):
#   jni/Android.mk          -> all-subdir-makefiles
#   jni/Application.mk
#   jni/SDL/Android.mk      -> PREBUILT_SHARED_LIBRARY SDL2
#   jni/SDL/include/…       -> SDL headers
#   jni/src/Android.mk      -> this file (LOCAL_MODULE := main)
#   jni/src/src/…           -> game sources (or CONSTRUO_SRC_ROOT override)
#   jni/src/external/…      -> vendored helpers
#
# LOCAL_SHARED_LIBRARIES := SDL2 is resolved via the sibling jni/SDL module;
# do NOT call import-module (that requires NDK_MODULE_PATH).

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Prefer staged tree under this directory (src/ + external/ copied here).
# Fallback: monorepo root five levels up when building from a git checkout
# without the Nix staging step (mk/android/app/jni/src -> repo root).
CONSTRUO_SRC_ROOT := $(LOCAL_PATH)
ifeq ($(wildcard $(CONSTRUO_SRC_ROOT)/src/main.cpp),)
  CONSTRUO_SRC_ROOT := $(LOCAL_PATH)/../../../../..
endif

LOCAL_SRC_FILES := \
  $(CONSTRUO_SRC_ROOT)/src/main.cpp \
  $(CONSTRUO_SRC_ROOT)/src/construo_main.cpp \
  $(CONSTRUO_SRC_ROOT)/src/sdl2_display.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gles2_renderer.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gl_api.cpp \
  $(CONSTRUO_SRC_ROOT)/src/unix_system.cpp \
  $(CONSTRUO_SRC_ROOT)/src/colors.cpp \
  $(CONSTRUO_SRC_ROOT)/src/command_line.cpp \
  $(CONSTRUO_SRC_ROOT)/src/construo.cpp \
  $(CONSTRUO_SRC_ROOT)/src/controller.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_button.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_directory.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_directory_button.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_file_button.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_file_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_label.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_new_file_button.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_widget.cpp \
  $(CONSTRUO_SRC_ROOT)/src/gui_window.cpp \
  $(CONSTRUO_SRC_ROOT)/src/input_context.cpp \
  $(CONSTRUO_SRC_ROOT)/src/load_gui_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/particle.cpp \
  $(CONSTRUO_SRC_ROOT)/src/particle_factory.cpp \
  $(CONSTRUO_SRC_ROOT)/src/path.cpp \
  $(CONSTRUO_SRC_ROOT)/src/path_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/reader.cpp \
  $(CONSTRUO_SRC_ROOT)/src/rect_collider.cpp \
  $(CONSTRUO_SRC_ROOT)/src/root_graphic_context.cpp \
  $(CONSTRUO_SRC_ROOT)/src/save_gui_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/screen_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/selection.cpp \
  $(CONSTRUO_SRC_ROOT)/src/settings.cpp \
  $(CONSTRUO_SRC_ROOT)/src/spring.cpp \
  $(CONSTRUO_SRC_ROOT)/src/widget_group.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_button.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_cache.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_gui_manager.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_reader.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_renderer.cpp \
  $(CONSTRUO_SRC_ROOT)/src/world_writer.cpp \
  $(CONSTRUO_SRC_ROOT)/src/worldview_collider_tool.cpp \
  $(CONSTRUO_SRC_ROOT)/src/worldview_insert_tool.cpp \
  $(CONSTRUO_SRC_ROOT)/src/worldview_select_tool.cpp \
  $(CONSTRUO_SRC_ROOT)/src/worldview_widget.cpp \
  $(CONSTRUO_SRC_ROOT)/src/worldview_zoom_tool.cpp \
  $(CONSTRUO_SRC_ROOT)/src/writer.cpp \
  $(CONSTRUO_SRC_ROOT)/src/zoom_graphic_context.cpp \
  $(CONSTRUO_SRC_ROOT)/src/zoom_state.cpp \
  $(CONSTRUO_SRC_ROOT)/external/logmich/src/log.cpp \
  $(CONSTRUO_SRC_ROOT)/external/logmich/src/logger.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/float.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/io.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/lexer.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/parser.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/util.cpp \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/src/value.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/override_reader_mapping.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/reader_collection.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/reader_document.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/reader_error.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/reader_mapping.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/reader_object.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/sexpr_reader_impl.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/sexpr_writer_impl.cpp \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/src/writer.cpp

LOCAL_C_INCLUDES := \
  $(CONSTRUO_SRC_ROOT)/src \
  $(CONSTRUO_SRC_ROOT)/external/logmich/include \
  $(CONSTRUO_SRC_ROOT)/external/sexpcpp/include \
  $(CONSTRUO_SRC_ROOT)/external/priocpp/include \
  $(CONSTRUO_SRC_ROOT)/external/geomcpp/include \
  $(LOCAL_PATH)/../SDL/include \
  $(LOCAL_PATH)/../SDL/include/SDL2

# glm headers: default to vendored external/glm, or GLM_ROOT override.
GLM_ROOT ?= $(CONSTRUO_SRC_ROOT)/external/glm
LOCAL_C_INCLUDES += $(GLM_ROOT)

ifndef CONSTRUO_VERSION
  CONSTRUO_VERSION := $(shell cat $(CONSTRUO_SRC_ROOT)/VERSION 2>/dev/null | tr -d '\n' || echo unknown)
endif
LOCAL_CFLAGS := -DUSE_SDL2_DISPLAY -DCONSTRUO_NO_XDGCPP -DPRIO_USE_SEXPCPP=1 \
  -DGLM_ENABLE_EXPERIMENTAL -DANDROID \
  -DCONSTRUO_VERSION=\"$(CONSTRUO_VERSION)\" -DVERSION=\"$(CONSTRUO_VERSION)\" \
  -DPACKAGE=\"construo\"
LOCAL_CPPFLAGS := -std=c++20 -frtti -fexceptions
LOCAL_LDLIBS := -lGLESv2 -llog -landroid

# Resolved via sibling jni/SDL (PREBUILT_SHARED_LIBRARY), not import-module.
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)
