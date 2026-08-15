// Construo - A wire-frame construction game
// Copyright (C) 2000–2026 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "path_manager.hpp"

#include <filesystem>

#include <logmich/log.hpp>

namespace construo {

PathManager path_manager;

namespace {

bool path_readable(std::filesystem::path const& p)
{
  std::error_code ec;
  // Prefer exists+is_regular_file/is_directory over POSIX access() for Win32/WASM.
  return std::filesystem::exists(p, ec) && !ec;
}

} // namespace

PathManager::PathManager () :
  m_path_list(),
  m_path_found(false),
  m_base_path()
{
}

PathManager::~PathManager ()
{
}

void
PathManager::add_path(std::string const& path)
{
  m_path_list.push_back(path);
}

std::filesystem::path
PathManager::complete(std::filesystem::path const& relative_path)
{
  return m_base_path / relative_path;
}

bool
PathManager::find_path(const std::list<std::filesystem::path>& file_list)
{
  for (auto i = m_path_list.begin (); !m_path_found && i != m_path_list.end (); ++i)
  {
    bool found_file = true;
    for (auto f = file_list.begin (); found_file && f != file_list.end (); ++f)
    {
      if (!path_readable(*i / *f))
        found_file = false;
    }
    if (found_file)
    {
      m_path_found = true;
      m_base_path = *i;

      log_info("PathManager: Using base_path: {}", m_base_path.string());

      return true;
    }
  }

  log_info("PathManager: No base path found");

  return false;
}

/** Search for a path which contains the file 'file' */
bool
PathManager::find_path(const std::filesystem::path& file)
{
  for (auto i = m_path_list.begin (); !m_path_found && i != m_path_list.end (); ++i)
  {
    if (path_readable(*i / file))
    {
      m_path_found = true;
      m_base_path = *i;

      log_info("PathManager: Using base_path: {}", m_base_path.string());

      return true;
    }
  }

  log_info("PathManager: No base path found");

  return false;
}

void
PathManager::set_path(const std::string& path)
{
  m_base_path = path;
}

} // namespace construo

/* EOF */
