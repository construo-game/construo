# Construo - A wire-frame construction game
# Copyright (C) 2015–2026 Ingo Ruhnke <grumbel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.14)
project(construo)

# Inlined CMake helpers (formerly provided by tinycmmc). No find_package(tinycmmc).
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

option(BUILD_TESTS "Build test cases" OFF)
option(CONSTRUO_USE_SDL2 "Build SDL2 + GLES2 backend" OFF)
option(CONSTRUO_USE_GLUT "Build GLUT/OpenGL backend" ON)
option(CONSTRUO_USE_X11 "Build X11 software backend" ON)
option(CONSTRUO_NO_XDGCPP "Omit xdgcpp (embedded / Win32 / Android)" OFF)

# Auto-disable xdgcpp on platforms where it is typically unavailable.
if(EMSCRIPTEN OR ANDROID OR WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(CONSTRUO_NO_XDGCPP ON CACHE BOOL "Omit xdgcpp (embedded / Win32 / Android)" FORCE)
endif()

if(WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # Force SDL2 on Windows: no X11/GLUT.
  set(CONSTRUO_USE_SDL2 ON CACHE BOOL "Build SDL2 + GLES2 backend" FORCE)
  set(CONSTRUO_USE_GLUT OFF CACHE BOOL "Build GLUT/OpenGL backend" FORCE)
  set(CONSTRUO_USE_X11 OFF CACHE BOOL "Build X11 software backend" FORCE)
endif()

include(GetProjectVersion)
message(STATUS "Version: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(GNUInstallDirs)
include(MaximumWarnings)

set(OpenGL_GL_PREFERENCE "GLVND")
if(CONSTRUO_USE_GLUT)
  find_package(OpenGL)
  find_package(GLUT)
endif()
if(CONSTRUO_USE_X11)
  find_package(X11)
endif()
if(CONSTRUO_USE_SDL2)
  find_package(SDL2 REQUIRED)
  # GLES2 headers (libGLESv2). On desktop, often provided by mesa/libglvnd.
  find_library(GLESV2_LIBRARY GLESv2)
  if(NOT GLESV2_LIBRARY)
    # Fall back to linking via SDL's GL loader / system GL for desktop GLES.
    message(STATUS "GLESv2 library not found by name; relying on SDL2 GL context")
  endif()
endif()
find_package(fmt REQUIRED)
find_package(glm REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_search_module(SIGCXX REQUIRED sigc++-3.0 IMPORTED_TARGET)

set(CONSTRUO_DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")

if(BUILD_TESTS)
  enable_testing()
endif()

# Build dependencies from external/ when not provided by the environment.
# (Subprojects may still use their own tinycmmc copies; construo itself does not.)
function(build_dependencies)
  set(BUILD_TESTS OFF)

  find_package(logmich QUIET)
  if(NOT TARGET logmich::logmich)
    add_subdirectory(external/logmich/ EXCLUDE_FROM_ALL)
  endif()

  find_package(prio QUIET)
  if(NOT TARGET prio::prio)
    add_subdirectory(external/priocpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(geom QUIET)
  if(NOT TARGET geom::geom)
    add_subdirectory(external/geomcpp/ EXCLUDE_FROM_ALL)
  endif()

  if(NOT CONSTRUO_NO_XDGCPP)
    find_package(xdgcpp QUIET)
    if(NOT TARGET xdgcpp)
      add_subdirectory(external/xdgcpp/ EXCLUDE_FROM_ALL)
    endif()
  endif()
endfunction()
build_dependencies()

set(CONSTRUO_SOURCES_CXX
  src/colors.cpp
  src/command_line.cpp
  src/construo.cpp
  src/controller.cpp
  src/cursors/cursor_collider.xbm
  src/cursors/cursor_collider_mask.xbm
  src/cursors/cursor_insert.xbm
  src/cursors/cursor_insert_mask.xbm
  src/cursors/cursor_scroll.xbm
  src/cursors/cursor_scroll_mask.xbm
  src/cursors/cursor_select.xbm
  src/cursors/cursor_select_mask.xbm
  src/cursors/cursor_zoom.xbm
  src/cursors/cursor_zoom_mask.xbm
  src/gui_button.cpp
  src/gui_directory.cpp
  src/gui_directory_button.cpp
  src/gui_file_button.cpp
  src/gui_file_manager.cpp
  src/gui_label.cpp
  src/gui_manager.cpp
  src/gui_new_file_button.cpp
  src/gui_widget.cpp
  src/gui_window.cpp
  src/input_context.cpp
  src/load_gui_manager.cpp
  src/particle.cpp
  src/particle_factory.cpp
  src/path.cpp
  src/path_manager.cpp
  src/reader.cpp
  src/rect_collider.cpp
  src/root_graphic_context.cpp
  src/save_gui_manager.cpp
  src/screen_manager.cpp
  src/selection.cpp
  src/settings.cpp
  src/spring.cpp
  src/widget_group.cpp
  src/world.cpp
  src/world_button.cpp
  src/world_cache.cpp
  src/world_gui_manager.cpp
  src/world_reader.cpp
  src/world_renderer.cpp
  src/world_writer.cpp
  src/worldview_collider_tool.cpp
  src/worldview_insert_tool.cpp
  src/worldview_select_tool.cpp
  src/worldview_widget.cpp
  src/worldview_zoom_tool.cpp
  src/writer.cpp
  src/zoom_graphic_context.cpp
  src/zoom_state.cpp
)

if(WIN32)
  list(APPEND CONSTRUO_SOURCES_CXX src/win32_system.cpp)
else()
  list(APPEND CONSTRUO_SOURCES_CXX src/unix_system.cpp)
endif()

set(CONSTRUO_GLUT_SOURCES_CXX
  src/main.cpp
  src/construo_main.cpp
  src/glut_display.cpp
)

set(CONSTRUO_X11_SOURCES_CXX
  src/main.cpp
  src/construo_main.cpp
  src/x11_display.cpp
)

set(CONSTRUO_SDL2_SOURCES_CXX
  src/main.cpp
  src/construo_main.cpp
  src/sdl2_display.cpp
  src/gles2_renderer.cpp
)

include_directories(src/)

add_definitions(-DVERSION="${PROJECT_VERSION}")
add_definitions(-DPACKAGE="construo")
add_definitions(-DPACKAGE_STRING="${PROJECT_VERSION}")
add_definitions(-DCONSTRUO_DATADIR="${CONSTRUO_DATADIR}")

add_library(libconstruo STATIC ${CONSTRUO_SOURCES_CXX})
set_target_properties(libconstruo PROPERTIES
  CXX_STANDARD 20
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF)
set_target_properties(libconstruo PROPERTIES OUTPUT_NAME construo)
if(CONSTRUO_NO_XDGCPP)
  target_compile_definitions(libconstruo PUBLIC -DCONSTRUO_NO_XDGCPP)
  target_link_libraries(libconstruo PUBLIC prio::prio geom::geom fmt::fmt PkgConfig::SIGCXX)
else()
  target_link_libraries(libconstruo PUBLIC prio::prio geom::geom fmt::fmt xdgcpp PkgConfig::SIGCXX)
endif()
target_compile_options(libconstruo PRIVATE ${WARNINGS_CXX_FLAGS})

if(CONSTRUO_USE_GLUT AND GLUT_FOUND AND OPENGL_FOUND)
  add_executable(construo.glut ${CONSTRUO_GLUT_SOURCES_CXX})
  set_target_properties(construo.glut PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(construo.glut PRIVATE ${WARNINGS_CXX_FLAGS})
  target_compile_definitions(construo.glut PUBLIC -DUSE_GLUT_DISPLAY)
  target_link_libraries(construo.glut libconstruo OpenGL::GL OpenGL::GLU GLUT::GLUT)
  install(TARGETS construo.glut RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(FILES construo.glut.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

if(CONSTRUO_USE_X11 AND X11_FOUND)
  add_executable(construo.x11 ${CONSTRUO_X11_SOURCES_CXX})
  set_target_properties(construo.x11 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(construo.x11 PRIVATE ${WARNINGS_CXX_FLAGS})
  target_compile_definitions(construo.x11 PUBLIC -DUSE_X11_DISPLAY)
  target_link_libraries(construo.x11 libconstruo prio::prio X11::X11)
  install(TARGETS construo.x11 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  install(FILES construo.x11.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

if(CONSTRUO_USE_SDL2 AND SDL2_FOUND)
  add_executable(construo.sdl ${CONSTRUO_SDL2_SOURCES_CXX})
  set_target_properties(construo.sdl PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(construo.sdl PRIVATE ${WARNINGS_CXX_FLAGS})
  target_compile_definitions(construo.sdl PUBLIC -DUSE_SDL2_DISPLAY)
  # Config package provides SDL2::SDL2; module package provides SDL2_LIBRARIES.
  if(TARGET SDL2::SDL2)
    target_link_libraries(construo.sdl PRIVATE libconstruo SDL2::SDL2)
  else()
    target_include_directories(construo.sdl PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(construo.sdl PRIVATE libconstruo ${SDL2_LIBRARIES})
  endif()
  if(GLESV2_LIBRARY)
    target_link_libraries(construo.sdl PRIVATE ${GLESV2_LIBRARY})
  endif()
  # Desktop may need EGL for GLES contexts; Windows needs opengl32 for the
  # compatibility fallback path.
  find_library(EGL_LIBRARY EGL)
  if(EGL_LIBRARY)
    target_link_libraries(construo.sdl PRIVATE ${EGL_LIBRARY})
  endif()
  if(WIN32)
    target_link_libraries(construo.sdl PRIVATE opengl32)
  endif()
  install(TARGETS construo.sdl RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  if(EXISTS "${CMAKE_SOURCE_DIR}/construo.sdl.desktop")
    install(FILES construo.sdl.desktop DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
  endif()

  if(EMSCRIPTEN)
    # Preloaded examples live at /examples in the virtual FS.
    target_compile_definitions(construo.sdl PRIVATE -DCONSTRUO_DATADIR="/")
    target_link_options(construo.sdl PRIVATE
      "SHELL:-sFULL_ES2=1"
      "SHELL:-sALLOW_MEMORY_GROWTH=1"
      "SHELL:-sDISABLE_EXCEPTION_CATCHING=0"
      -fexceptions
    )
    # Prefer the wasm shell when present (browser builds).
    if(EXISTS "${CMAKE_SOURCE_DIR}/mk/wasm/shell.html")
      target_link_options(construo.sdl PRIVATE
        "SHELL:--shell-file=${CMAKE_SOURCE_DIR}/mk/wasm/shell.html"
      )
    endif()
    if(EXISTS "${CMAKE_SOURCE_DIR}/examples")
      target_link_options(construo.sdl PRIVATE
        "SHELL:--preload-file=${CMAKE_SOURCE_DIR}/examples@/examples"
      )
    endif()
  endif()
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CONSTRUO_DATADIR})

install(FILES
  construo.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

if(BUILD_TESTS)
  find_package(GTest REQUIRED)
  file(GLOB TEST_CONSTRUO_SOURCES test/*_test.cpp)
  add_executable(test_construo ${TEST_CONSTRUO_SOURCES})
  target_compile_options(test_construo PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_construo
    GTest::GTest
    GTest::Main
    libconstruo)
  enable_testing()
  add_test(NAME test_construo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test_construo)
endif()

# EOF #
