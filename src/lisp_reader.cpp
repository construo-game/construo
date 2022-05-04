// Construo - A wire-frame construction game
// Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
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

#include <string.h>
#include <span>

#include "construo_error.hpp"
#include "lisp_reader.hpp"

namespace prio {

template<>
bool read_custom(prio::ReaderMapping const& map, std::string_view key, Vector2d& value_out)
{
  std::vector<float> v;
  if (!map.read(key, v)) {
    return false;
  }

  if (v.size() < 2) {
    return false;
  }

  value_out = Vector2d(v[0], v[1]);
  return true;
}

} // namespace prio

/* EOF */
