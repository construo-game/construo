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

/** A collection of small math helper functions, some of them might be
    equal in functionality to standard STL functions, but provided
    here for portability and broken STL implementations

    @brief A collection of mathematical helper functions */
namespace Math {

const float pi   = 3.14159265358979323846f;	/* pi */
const float pi_2 = 1.57079632679489661923f;	/* pi/2 */

template<class T>
T min (const T& a, const T& b)
{
  if (a < b)
    return a;
  else
    return b;
}

template<class T>
T max (const T& a, const T& b)
{
  if (a > b)
    return a;
  else
    return b;
}

template<class T>
T mid (const T& a, const T& b, const T& c)
{
  return max((a), min((b), (c)));
}

inline int round(float a)
{
  return int((a > 0.0f) ? (a + 0.5f) : (a - 0.5f));
}

/** Round x to a multilple of n */
inline int round_to(float x, int n)
{
  if (x > 0.0f)
    return static_cast<int>(x + (static_cast<float>(n) / 2.0f)) / n * n;
  else
    return static_cast<int>(x - (static_cast<float>(n) / 2.0f)) / n * n;
}

/** Same as above, except n can be less to 1 */
inline float round_to_float(float x, float n)
{
  if (x > 0.0f)
    return int((x + (n / 2.0f)) / n) * n;
  else
    return int((x - (n / 2.0f)) / n) * n;
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

#endif
