# tinycmmc - Tiny CMake module collection
# Copyright (C) 2022 Ingo Ruhnke <grumbel@gmail.com>
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

# Bootstrap file that is looking for where tinycmmc is installed

find_package(tinycmmc CONFIG)
if(tinycmmc_FOUND)
  message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
  list(APPEND CMAKE_MODULE_PATH ${TINYCMMC_MODULE_PATH})
else()
  # Nested submodule, or monorepo sibling (e.g. construo/external/{geomcpp,tinycmmc}).
  set(_tinycmmc_candidates
    "${CMAKE_CURRENT_SOURCE_DIR}/external/tinycmmc"
    "${CMAKE_CURRENT_SOURCE_DIR}/../tinycmmc")
  set(_tinycmmc_root "")
  foreach(_cand IN LISTS _tinycmmc_candidates)
    if(EXISTS "${_cand}/CMakeLists.txt" OR EXISTS "${_cand}/modules")
      set(_tinycmmc_root "${_cand}")
      break()
    endif()
  endforeach()
  if(NOT _tinycmmc_root)
    message(FATAL_ERROR
      "tinycmmc could not be found (tried external/tinycmmc and ../tinycmmc). "
      "Run: git submodule update --init --recursive\n")
  endif()
  set(TINYCMMC_MODULE_PATH "${_tinycmmc_root}/modules/")
  message(STATUS "tinycmmc module path: ${TINYCMMC_MODULE_PATH}")
  list(APPEND CMAKE_MODULE_PATH "${TINYCMMC_MODULE_PATH}")
endif()

include(TinyCMMC)

# EOF #
