// Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_WORLD_CACHE_HPP
#define HEADER_CONSTRUO_WORLD_CACHE_HPP

#include <memory>
#include <string>
#include <unordered_map>

#include "fwd.hpp"

class WorldCache
{
public:
  WorldCache();

  /** Return either a World or nullptr in case of failure to load the World */
  World const* get(std::string const& filename);

private:
  std::unordered_map<std::string, std::unique_ptr<World>> m_worlds;

private:
  WorldCache(const WorldCache&) = delete;
  WorldCache& operator=(const WorldCache&) = delete;
};

#endif

/* EOF */
