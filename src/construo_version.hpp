// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_CONSTRUO_VERSION_HPP
#define HEADER_CONSTRUO_CONSTRUO_VERSION_HPP

// Defined by CMake from PROJECT_VERSION_FULL (see cmake/GetProjectVersion.cmake
// and the top-level VERSION file). Packaging may override via
// -DPROJECT_VERSION_FULL=... / Android.mk LOCAL_CFLAGS.
#ifndef CONSTRUO_VERSION
#  define CONSTRUO_VERSION "unknown"
#endif

#ifndef VERSION
#  define VERSION CONSTRUO_VERSION
#endif

#ifndef PACKAGE
#  define PACKAGE "construo"
#endif

#ifndef PACKAGE_STRING
#  define PACKAGE_STRING PACKAGE " " CONSTRUO_VERSION
#endif

// Install/datadir prefix; Android/WASM often use "/" or app storage instead.
#ifndef CONSTRUO_DATADIR
#  define CONSTRUO_DATADIR "."
#endif

#endif

/* EOF */
