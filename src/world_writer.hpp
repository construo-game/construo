// Construo - A wire-frame construction game
// Copyright (C) 2002-2022 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_CONSTRUO_WORLD_WRITER_HPP
#define HEADER_CONSTRUO_WORLD_WRITER_HPP

#include <string>

#include "fwd.hpp"

namespace construo {

class WorldWriter
{
public:
  WorldWriter(World const& world);

  void write(std::string const& filename);

private:
  World& m_world;

private:
  WorldWriter(const WorldWriter&) = delete;
  WorldWriter& operator=(const WorldWriter&) = delete;
};

} // namespace construo

#endif

/* EOF */
