// Construo - A wire-frame construction game
// Copyright (C) 2022 Ingo Ruhnke <grumbel@gmail.com>
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

#include "path.hpp"

std::string path_join(std::string const& lhs, std::string const& rhs)
{
  if (lhs.empty()) {
    return rhs;
  } else if (!rhs.empty() && rhs.front() == '/') {
    return rhs;
  } else {
    if (lhs.back() == '/') {
      return lhs + rhs;
    } else {
      return lhs + "/" + rhs;
    }
  }
}

std::string path_dirname(std::string const& path)
{
  if (path.empty()) {
    return path;
  } else if (path == "/") {
    return "/";
  } else {
    size_t const p = path.find_last_of('/');
    if (p == std::string::npos) {
      return {};
    } else if (p == 0) {
      return "/";
    } else {
      return path.substr(0, p);
    }
  }
}

std::string path_basename(std::string const& path)
{
  size_t const p = path.find_last_of('/');
  if (p == std::string::npos) {
    return path;
  } else {
    return path.substr(p + 1);
  }
}

/* EOF */
