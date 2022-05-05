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

#include "world_cache.hpp"

#include <iostream>

#include "world.hpp"

WorldCache::WorldCache() :
  m_worlds()
{
}

World const*
WorldCache::get(std::string const& filename)
{
  if (auto it = m_worlds.find(filename); it != m_worlds.end()) {
    return it->second.get();
  } else {
    try {
      auto world = std::make_unique<World>(filename);
      World const* tmp = world.get();
      m_worlds[filename] = std::move(world);
      return tmp;
    } catch (std::exception const& err) {
      std::cerr << filename << ": failed to load World: " << err.what() << std::endl;
      m_worlds[filename] = {};
      return nullptr;
    }
  }
}

/* EOF */
