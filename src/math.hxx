//  $Id$
// 
//  Construo - A wire-frame construction gamee
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef CONSTRUO_MATH_HXX
#define CONSTRUO_MATH_HXX

/** A collection of small math helper functions, some of them might be
    equal in functionality to standard STL functions, but provided
    here for portability and broken STL implementations 
    
    @brief A collection of mathematical helper functions */
namespace Math {

const double pi   = 3.14159265358979323846;	/* pi */
const double pi_2 = 1.57079632679489661923;	/* pi/2 */

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
  return int((a > 0) ? (a + .5f) : (a - .5));
}

/** Round x to a multilple of n */
inline int round_to(float x, int n)
{
  if (x > 0)
    return static_cast<int>(x + (n/2)) / n * n;
  else
    return static_cast<int>(x - (n/2)) / n * n;
}

/** Same as above, except n can be less to 1 */
inline float round_to_float(float x, float n)
{
  if (x > 0)
    return int((x + (n/2)) / n) * n;
  else
    return int((x - (n/2)) / n) * n;
}

/** Get exponent of x */
inline int get_exp_n(float x, int n)
{
  int e = 0;
  while(x < 1)
  {
    x *= n;
    e--;
  }
  while(x >= n)
  {
    x /= n;
    e++;
  }
  return e;
}

} // namespace Math

#endif
