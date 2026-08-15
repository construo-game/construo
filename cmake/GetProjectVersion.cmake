# SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
# SPDX-License-Identifier: GPL-3.0-or-later
#
# Version is defined solely by the top-level VERSION file.
# Development versions contain a "-dev" suffix and, when built from a git
# checkout (or when packaging supplies PROJECT_VERSION_FULL), append
# .<revCount>+g<shortHash>[-dirty].

# Packaging (e.g. Nix) may pass -DPROJECT_VERSION_FULL=... with the full
# development or release string already expanded.
if(NOT DEFINED PROJECT_VERSION_FULL OR PROJECT_VERSION_FULL STREQUAL "")
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/VERSION")
    message(FATAL_ERROR "VERSION file missing at ${CMAKE_SOURCE_DIR}/VERSION")
  endif()
  file(STRINGS "${CMAKE_SOURCE_DIR}/VERSION" PROJECT_VERSION_FULL LIMIT_COUNT 1)
  string(STRIP "${PROJECT_VERSION_FULL}" PROJECT_VERSION_FULL)

  # Expand development builds from a git working tree.
  if(PROJECT_VERSION_FULL MATCHES "-dev" AND EXISTS "${CMAKE_SOURCE_DIR}/.git")
    find_package(Git QUIET)
    if(Git_FOUND)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-list --count HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE _git_rev_count
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" rev-parse --short HEAD
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE _git_short
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
      execute_process(
        COMMAND "${GIT_EXECUTABLE}" status --porcelain
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        OUTPUT_VARIABLE _git_status
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
      if(_git_rev_count AND _git_short)
        set(PROJECT_VERSION_FULL "${PROJECT_VERSION_FULL}.${_git_rev_count}+g${_git_short}")
        if(_git_status)
          set(PROJECT_VERSION_FULL "${PROJECT_VERSION_FULL}-dirty")
        endif()
      endif()
    endif()
  endif()
endif()

# CMake project() / packaging numeric version: leading X.Y.Z only.
string(REGEX MATCH "^[0-9]+(\\.[0-9]+)*" PROJECT_VERSION "${PROJECT_VERSION_FULL}")
if(NOT PROJECT_VERSION)
  set(PROJECT_VERSION "0.0.0")
endif()

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Version: ${PROJECT_VERSION_FULL}")
