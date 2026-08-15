// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_WIN32_SYSTEM_HPP
#define HEADER_CONSTRUO_WIN32_SYSTEM_HPP

#include <filesystem>
#include <string>
#include <vector>

#include "system_context.hpp"

namespace construo {

/** SystemContext for Windows (MinGW / MSVC). Uses std::filesystem and
    Win32 environment variables; no POSIX or xdgcpp. */
class Win32System : public SystemContext
{
public:
  Win32System();
  ~Win32System() override;

  unsigned long get_time() override;
  void sleep(unsigned int msec) override;

  std::filesystem::path get_construo_rc_path() override;
  std::string get_user_realname() override;
  std::string get_user_email() override;

  FileType get_file_type(const std::string& filename) override;
  unsigned long get_mtime(const std::string& filename) override;

  FILE* open_input_file(const std::string& filename) override;
  FILE* open_output_file(const std::string& filename) override;

  std::vector<std::string> read_directory(const std::string& pathname) override;
  std::filesystem::path translate_filename(const std::string& filename) override;

private:
  unsigned long m_start_time;
  std::filesystem::path m_construo_rc_path;
};

} // namespace construo

#endif

/* EOF */
