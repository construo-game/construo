# Construo - A wire-frame construction game
# Copyright (C) 2015 Ingo Ruhnke <grumbel@gmail.com>
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

cmake_minimum_required(VERSION 3.0)
project(construo)

set(TINYCMMC_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/external/cmake-modules/modules/")
find_package(tinycmmc CONFIG)
message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})

option(BUILD_TESTS "Build test cases" OFF)

include(GetProjectVersion)
message(STATUS "Version: ${PROJECT_NAME} ${PROJECT_VERSION}")

include(GNUInstallDirs)
include(MaximumWarnings)

set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL)
find_package(GLUT)
find_package(X11)

set(CONSTRUO_DATADIR "${CMAKE_INSTALL_FULL_DATADIR}/${PROJECT_NAME}")

if (BUILD_TESTS)
  enable_testing()
endif(BUILD_TESTS)

# Build dependencies
function(build_dependencies)
  set(BUILD_TESTS OFF)

  find_package(prio QUIET)
  if(NOT TARGET prio::prio)
    add_subdirectory(external/priocpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(prio QUIET)
  if(NOT TARGET prio::prio)
    add_subdirectory(external/priocpp/ EXCLUDE_FROM_ALL)
  endif()

  find_package(xdgcpp QUIET)
  if(NOT TARGET xdgcpp)
    add_subdirectory(external/xdgcpp/ EXCLUDE_FROM_ALL)
  endif()
endfunction()
build_dependencies()

set(CONSTRUO_SOURCES_CXX
  src/bounding_box.cpp
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
  src/gui_buttons.cpp
  src/gui_child_manager.cpp
  src/gui_component.cpp
  src/gui_directory.cpp
  src/gui_directory_button.cpp
  src/gui_file_button.cpp
  src/gui_file_manager.cpp
  src/gui_label.cpp
  src/gui_manager.cpp
  src/gui_new_file_button.cpp
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
  src/string_utils.cpp
  src/unix_system.cpp
  src/world.cpp
  src/world_button.cpp
  src/world_cache.cpp
  src/world_gui_manager.cpp
  src/worldview_collider_tool.cpp
  src/worldview_component.cpp
  src/worldview_insert_tool.cpp
  src/worldview_select_tool.cpp
  src/worldview_tool.cpp
  src/worldview_zoom_tool.cpp
  src/writer.cpp
  src/zoom_graphic_context.cpp
  src/zoom_state.cpp
  )

set(CONSTRUO_GLUT_SOURCES_CXX
  src/construo_main.cpp
  src/glut_display.cpp
  )

set(CONSTRUO_X11_SOURCES_CXX
  src/construo_main.cpp
  src/x11_display.cpp
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
target_link_libraries(libconstruo PUBLIC prio::prio xdgcpp)
target_compile_options(libconstruo PRIVATE ${WARNINGS_CXX_FLAGS} -Wuseless-cast)

if(GLUT_FOUND AND OPENGL_FOUND)
  add_executable(construo.glut ${CONSTRUO_GLUT_SOURCES_CXX})
  set_target_properties(construo.glut PROPERTIES
      CXX_STANDARD 20
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF)
  target_compile_options(construo.glut PRIVATE ${WARNINGS_CXX_FLAGS})
  target_compile_definitions(construo.glut PUBLIC -DUSE_GLUT_DISPLAY)
  target_include_directories(construo.glut SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIRS})
  target_link_libraries(construo.glut libconstruo ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})

  install(TARGETS construo.glut RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  install(FILES
    construo.glut.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

if(X11_FOUND)
  add_executable(construo.x11 ${CONSTRUO_X11_SOURCES_CXX})
  set_target_properties(construo.x11 PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF)
  target_compile_options(construo.x11 PRIVATE ${WARNINGS_CXX_FLAGS})
  target_compile_definitions(construo.x11 PUBLIC -DUSE_X11_DISPLAY)
  target_include_directories(construo.x11 SYSTEM PUBLIC ${X11_INCLUDE_DIR})
  target_link_libraries(construo.x11 libconstruo prio::prio ${X11_LIBRARIES})

  if(X11_Xext_FOUND)
    target_link_libraries(construo.x11 ${X11_Xext_LIB})
  endif()

  install(TARGETS construo.x11 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

  install(FILES
    construo.x11.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications)
endif()

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CONSTRUO_DATADIR})

install(FILES
  construo.svg
  DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps)

configure_file(construo.appdata.xml.in ${CMAKE_CURRENT_BINARY_DIR}/construo.appdata.xml)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/construo.appdata.xml
  DESTINATION ${CMAKE_INSTALL_DATADIR}/metainfo)

if (BUILD_TESTS)
  add_test(NAME construo.appdata.xml
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND appstream-util validate-relax ${CMAKE_CURRENT_BINARY_DIR}/construo.appdata.xml)

  # compile tests
  find_package(GTest REQUIRED)

  # build automatic tests
  file(GLOB TEST_CONSTRUO_SOURCES test/*_test.cpp)
  add_executable(test_construo ${TEST_CONSTRUO_SOURCES})
  target_compile_options(test_construo PRIVATE ${WARNINGS_CXX_FLAGS})
  target_link_libraries(test_construo
    GTest::GTest
    GTest::Main
    libconstruo)

  # add 'make test' target, use 'make test ARGS="-V"' or 'ctest -V' for verbose
  enable_testing()
  add_test(NAME test_construo
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND test_construo)
endif()

# EOF #
