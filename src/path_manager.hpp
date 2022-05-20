// Pingus - A free Lemmings clone
// Copyright (C) 2000 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_PINGUS_PATH_MANAGER_HPP
#define HEADER_PINGUS_PATH_MANAGER_HPP

#include <filesystem>
#include <string>
#include <list>

namespace construo {

class PathManager
{
public:
  PathManager ();
  ~PathManager ();

  /** Search for a path which contains all the files given in
      file_list */
  bool find_path(std::list<std::filesystem::path> const& file_list);

  /** Search for a path which contains the file 'file' */
  bool find_path(std::filesystem::path const& path);

  /** Set the path directly without using find_path/add_path
      Must include trailing slash */
  void set_path(std::string const& path);

  /** Add a path to the search list */
  void add_path(std::string const& path);

  std::filesystem::path get_base_path() const { return m_base_path; }

  /** Complete a releative path to the absolute path, the returned
      path contains a trailing slash */
  std::filesystem::path complete(std::filesystem::path const& relative_path);

private:
  std::list<std::string> m_path_list;
  bool m_path_found;
  std::filesystem::path m_base_path;

private:
  PathManager (const PathManager&);
  PathManager& operator= (const PathManager&);
};

extern PathManager path_manager;

} // namespace construo

#endif

/* EOF */
