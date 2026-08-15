// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "win32_system.hpp"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <logmich/log.hpp>

#include "construo.hpp"
#include "construo_error.hpp"
#include "path.hpp"
#include "path_manager.hpp"

#ifdef _WIN32
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#endif

namespace construo {

namespace {

unsigned long now_ms()
{
  using namespace std::chrono;
  return static_cast<unsigned long>(
    duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count());
}

std::filesystem::path config_home()
{
  if (char const* appdata = std::getenv("APPDATA")) {
    return std::filesystem::path(appdata) / "construo";
  }
  if (char const* profile = std::getenv("USERPROFILE")) {
    return std::filesystem::path(profile) / "AppData" / "Roaming" / "construo";
  }
  return std::filesystem::path(".") / "construo-config";
}

} // namespace

Win32System::Win32System() :
  m_start_time(now_ms()),
  m_construo_rc_path(config_home())
{
  if (!std::filesystem::is_directory(m_construo_rc_path)) {
    std::error_code ec;
    std::filesystem::create_directories(m_construo_rc_path, ec);
    if (ec) {
      throw ConstruoError("Win32System: could not create config directory: " +
                          m_construo_rc_path.string());
    }
  }
}

Win32System::~Win32System() = default;

unsigned long
Win32System::get_time()
{
  return now_ms() - m_start_time;
}

void
Win32System::sleep(unsigned int msec)
{
#ifdef _WIN32
  Sleep(msec);
#else
  std::this_thread::sleep_for(std::chrono::milliseconds(msec));
#endif
}

std::filesystem::path
Win32System::get_construo_rc_path()
{
  return m_construo_rc_path;
}

std::string
Win32System::get_user_realname()
{
  if (char const* name = std::getenv("USERNAME")) {
    return name;
  }
  return {};
}

std::string
Win32System::get_user_email()
{
  if (char const* email = std::getenv("EMAIL")) {
    return email;
  }
  return {};
}

std::filesystem::path
Win32System::translate_filename(const std::string& filename)
{
  if (filename == "/") {
    return {};
  } else if (filename == "/user") {
    return m_construo_rc_path;
  } else if (filename == "/examples") {
    return path_manager.complete("examples");
  } else if (filename.starts_with("/user/")) {
    return m_construo_rc_path / filename.substr(6);
  } else if (filename.starts_with("/examples/")) {
    return path_manager.complete("examples") / filename.substr(10);
  }
  return filename;
}

unsigned long
Win32System::get_mtime(const std::string& filename)
{
  std::error_code ec;
  auto ft = std::filesystem::last_write_time(translate_filename(filename), ec);
  if (ec) {
    return 0;
  }
  // file_clock epoch is implementation-defined; expose a monotonic-ish stamp
  // for cache invalidation only (not wall-clock).
  return static_cast<unsigned long>(ft.time_since_epoch().count());
}

FileType
Win32System::get_file_type(const std::string& filename)
{
  if (filename == "/examples" || filename == "/user") {
    return FT_DIRECTORY;
  }

  std::filesystem::path sys_name = translate_filename(filename);
  std::error_code ec;
  if (std::filesystem::is_directory(sys_name, ec)) {
    return FT_DIRECTORY;
  }
  if (std::filesystem::is_regular_file(sys_name, ec)) {
    if (filename.ends_with(".construo") || filename.ends_with(".construo.gz")) {
      return FT_CONSTRUO_FILE;
    }
    return FT_UNKNOWN_FILE;
  }
  log_debug("Win32System: could not stat '{}'", sys_name.string());
  return FT_UNKNOWN_FILE;
}

FILE*
Win32System::open_input_file(const std::string& filename)
{
  return fopen(translate_filename(filename).string().c_str(), "rb");
}

FILE*
Win32System::open_output_file(const std::string& filename)
{
  return fopen(translate_filename(filename).string().c_str(), "wb");
}

std::vector<std::string>
Win32System::read_directory(const std::string& arg_pathname)
{
  if (arg_pathname == "/") {
    return {"examples", "user"};
  }

  std::vector<std::string> dir_lst;
  std::filesystem::path pathname = translate_filename(arg_pathname);
  std::error_code ec;
  for (auto const& entry : std::filesystem::directory_iterator{pathname, ec}) {
    auto name = entry.path().filename().string();
    if (name != "." && name != "..") {
      dir_lst.push_back(name);
    }
  }

  std::sort(dir_lst.begin(), dir_lst.end(),
            [&](std::string const& lhs, std::string const& rhs) {
              FileType lt = g_system_context->get_file_type(path_join(arg_pathname, lhs));
              FileType rt = g_system_context->get_file_type(path_join(arg_pathname, rhs));
              if (lt == rt) {
                return lhs < rhs;
              }
              if (lt == FT_DIRECTORY) {
                return true;
              }
              if (rt == FT_DIRECTORY) {
                return false;
              }
              return lhs < rhs;
            });
  return dir_lst;
}

} // namespace construo

/* EOF */
