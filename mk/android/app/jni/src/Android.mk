# Android.mk for Construo libmain (SDL2 + GLES2).
# Expected layout after staging (see nix/android.nix / Pingus build-apk.sh):
#   jni/Android.mk          -> all-subdir-makefiles
#   jni/Application.mk
#   jni/SDL/Android.mk      -> PREBUILT_SHARED_LIBRARY SDL2
#   jni/SDL/include/…       -> SDL headers
#   jni/src/Android.mk      -> this file (LOCAL_MODULE := main)
#   jni/src/src/…           -> game sources
#   jni/src/external/…      -> vendored helpers
#
# LOCAL_SRC_FILES must be relative to LOCAL_PATH. ndk-build prepends
# LOCAL_PATH itself — never put $(LOCAL_PATH)/… in LOCAL_SRC_FILES or
# paths double (e.g. jni/src/jni/src/src/main.cpp).
#
# LOCAL_SHARED_LIBRARIES := SDL2 is resolved via the sibling jni/SDL module;
# do NOT call import-module (that requires NDK_MODULE_PATH).

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

# Staged tree: jni/src/{src,external}/…  (Nix mkApk / package scripts)
# Checkout without staging: sources live at repo root five levels up from
# mk/android/app/jni/src → use relative prefix ../../../../
SRC_REL :=
ifeq ($(wildcard $(LOCAL_PATH)/src/main.cpp),)
  # jni/src -> jni -> app -> android -> mk -> repo root (5 levels)
  SRC_REL := ../../../../../
endif

LOCAL_SRC_FILES := \
  $(SRC_REL)src/main.cpp \
  $(SRC_REL)src/construo_main.cpp \
  $(SRC_REL)src/sdl2_display.cpp \
  $(SRC_REL)src/gles2_renderer.cpp \
  $(SRC_REL)src/gl_api.cpp \
  $(SRC_REL)src/unix_system.cpp \
  $(SRC_REL)src/colors.cpp \
  $(SRC_REL)src/command_line.cpp \
  $(SRC_REL)src/construo.cpp \
  $(SRC_REL)src/controller.cpp \
  $(SRC_REL)src/gui_button.cpp \
  $(SRC_REL)src/gui_directory.cpp \
  $(SRC_REL)src/gui_directory_button.cpp \
  $(SRC_REL)src/gui_file_button.cpp \
  $(SRC_REL)src/gui_file_manager.cpp \
  $(SRC_REL)src/gui_label.cpp \
  $(SRC_REL)src/gui_manager.cpp \
  $(SRC_REL)src/gui_new_file_button.cpp \
  $(SRC_REL)src/gui_widget.cpp \
  $(SRC_REL)src/gui_window.cpp \
  $(SRC_REL)src/input_context.cpp \
  $(SRC_REL)src/load_gui_manager.cpp \
  $(SRC_REL)src/particle.cpp \
  $(SRC_REL)src/particle_factory.cpp \
  $(SRC_REL)src/path.cpp \
  $(SRC_REL)src/path_manager.cpp \
  $(SRC_REL)src/reader.cpp \
  $(SRC_REL)src/rect_collider.cpp \
  $(SRC_REL)src/root_graphic_context.cpp \
  $(SRC_REL)src/save_gui_manager.cpp \
  $(SRC_REL)src/screen_manager.cpp \
  $(SRC_REL)src/selection.cpp \
  $(SRC_REL)src/settings.cpp \
  $(SRC_REL)src/spring.cpp \
  $(SRC_REL)src/widget_group.cpp \
  $(SRC_REL)src/world.cpp \
  $(SRC_REL)src/world_button.cpp \
  $(SRC_REL)src/world_cache.cpp \
  $(SRC_REL)src/world_gui_manager.cpp \
  $(SRC_REL)src/world_reader.cpp \
  $(SRC_REL)src/world_renderer.cpp \
  $(SRC_REL)src/world_writer.cpp \
  $(SRC_REL)src/worldview_collider_tool.cpp \
  $(SRC_REL)src/worldview_insert_tool.cpp \
  $(SRC_REL)src/worldview_select_tool.cpp \
  $(SRC_REL)src/worldview_widget.cpp \
  $(SRC_REL)src/worldview_zoom_tool.cpp \
  $(SRC_REL)src/writer.cpp \
  $(SRC_REL)src/zoom_graphic_context.cpp \
  $(SRC_REL)src/zoom_state.cpp \
  $(SRC_REL)external/logmich/src/log.cpp \
  $(SRC_REL)external/logmich/src/logger.cpp \
  $(SRC_REL)external/sexpcpp/src/float.cpp \
  $(SRC_REL)external/sexpcpp/src/io.cpp \
  $(SRC_REL)external/sexpcpp/src/lexer.cpp \
  $(SRC_REL)external/sexpcpp/src/parser.cpp \
  $(SRC_REL)external/sexpcpp/src/util.cpp \
  $(SRC_REL)external/sexpcpp/src/value.cpp \
  $(SRC_REL)external/priocpp/src/override_reader_mapping.cpp \
  $(SRC_REL)external/priocpp/src/reader_collection.cpp \
  $(SRC_REL)external/priocpp/src/reader_document.cpp \
  $(SRC_REL)external/priocpp/src/reader_error.cpp \
  $(SRC_REL)external/priocpp/src/reader_mapping.cpp \
  $(SRC_REL)external/priocpp/src/reader_object.cpp \
  $(SRC_REL)external/priocpp/src/sexpr_reader_impl.cpp \
  $(SRC_REL)external/priocpp/src/sexpr_writer_impl.cpp \
  $(SRC_REL)external/priocpp/src/writer.cpp

# Includes may use absolute paths via LOCAL_PATH (not doubled by ndk-build).
CONSTRUO_INC_ROOT := $(LOCAL_PATH)
ifeq ($(wildcard $(LOCAL_PATH)/src/main.cpp),)
  CONSTRUO_INC_ROOT := $(LOCAL_PATH)/../../../../..
endif

LOCAL_C_INCLUDES := \
  $(CONSTRUO_INC_ROOT)/src \
  $(CONSTRUO_INC_ROOT)/external/logmich/include \
  $(CONSTRUO_INC_ROOT)/external/sexpcpp/include \
  $(CONSTRUO_INC_ROOT)/external/priocpp/include \
  $(CONSTRUO_INC_ROOT)/external/geomcpp/include \
  $(LOCAL_PATH)/../SDL/include \
  $(LOCAL_PATH)/../SDL/include/SDL2

# glm headers: default to vendored external/glm, or GLM_ROOT override.
GLM_ROOT ?= $(CONSTRUO_INC_ROOT)/external/glm
LOCAL_C_INCLUDES += $(GLM_ROOT)

ifndef CONSTRUO_VERSION
  CONSTRUO_VERSION := $(shell cat $(CONSTRUO_INC_ROOT)/VERSION 2>/dev/null | tr -d '\n' || echo unknown)
endif
LOCAL_CFLAGS := -DUSE_SDL2_DISPLAY -DCONSTRUO_NO_XDGCPP -DPRIO_USE_SEXPCPP=1 \
  -DGLM_ENABLE_EXPERIMENTAL -DANDROID \
  -DCONSTRUO_VERSION=\"$(CONSTRUO_VERSION)\" -DVERSION=\"$(CONSTRUO_VERSION)\" \
  -DPACKAGE=\"construo\" -DPACKAGE_STRING=\"construo $(CONSTRUO_VERSION)\" \
  -DCONSTRUO_DATADIR=\"/\"
LOCAL_CPPFLAGS := -std=c++20 -frtti -fexceptions
LOCAL_LDLIBS := -lGLESv2 -llog -landroid

# Resolved via sibling jni/SDL (PREBUILT_SHARED_LIBRARY), not import-module.
LOCAL_SHARED_LIBRARIES := SDL2

include $(BUILD_SHARED_LIBRARY)
