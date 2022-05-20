// Construo - A wire-frame construction gamee
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

#ifndef CONSTRUO_MATH_HPP
#define CONSTRUO_MATH_HPP

#include <cassert>
#include <cmath>

#include <geom/point.hpp>
#include <geom/size.hpp>

namespace construo {

/** A collection of small math helper functions, some of them might be
    equal in functionality to standard STL functions, but provided
    here for portability and broken STL implementations

    @brief A collection of mathematical helper functions */
namespace Math {

/** Round x to a multilple of n */
inline int round_to(float x, int n)
{
  if (n == 0) { return static_cast<int>(std::lround(x)); }

  return static_cast<int>(std::lround(x / static_cast<float>(n)) * n);
}

/** Same as above, except n can be less to 1 */
inline float round_to_float(float x, float n)
{
  if (n == 0) { return x; }

  return std::round(x / n) * n;
}

inline
geom::fpoint round_to(geom::fpoint const& point, geom::fsize const& size)
{
  return geom::fpoint(round_to_float(point.x(), size.width()),
                      round_to_float(point.y(), size.height()));
}

/** Get exponent of x */
inline int get_exp_n(float x, int n)
{
  int e = 0;
  while(x < 1)
  {
    x *= static_cast<float>(n);
    e--;
  }
  while(x >= static_cast<float>(n))
  {
    x /= static_cast<float>(n);
    e++;
  }
  return e;
}

} // namespace Math

} // namespace construo

#endif

/* EOF */
