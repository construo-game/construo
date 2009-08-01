//  $Id$
// 
//  Construo - A wire-frame construction gamee
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_RECT_HPP
#define HEADER_RECT_HPP

#include "math.hpp"
#include "vector2d.hpp"

/** */
template<class T>
class Rect
{
private:
public:
  T x1;
  T y1;
  T x2;
  T y2;
  
  Rect () 
  {
  }

  Rect (const T& x1_,
        const T& y1_,
        const T& x2_,
        const T& y2_)
    : x1 (Math::min(x1_, x2_)), 
      y1 (Math::min(y1_, y2_)), 
      x2 (Math::max(x1_, x2_)), 
      y2 (Math::max(y1_, y2_))
  {}

  T get_width () 
  {
    return x2 - x1;
  }

  T get_height () 
  {
    return x2 - x1;
  }

  Vector2d get_center () const 
  {
    return Vector2d ((x1 + x2)/2.0f,
                     (y1 + y2)/2.0f);
  }
};

#endif

/* EOF */
