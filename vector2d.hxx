//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
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

#ifndef HEADER_VECTOR2D_HXX
#define HEADER_VECTOR2D_HXX

#include <iostream>

/** */
class Vector2d
{
public:
  float x;
  float y;
  
  Vector2d ()
    : x(0), y(0)
  {}

  Vector2d (float x_, float y_) 
    : x (x_), y (y_)
  {}

  inline 
  void operator+= (const Vector2d& vec) {
    x += vec.x;
    y += vec.y;
  }

  inline 
  void operator-= (const Vector2d& vec) {
    x -= vec.x;
    y -= vec.y;
  }

  inline 
  void operator*= (float f) {
    x *= f;
    y *= f;
  }

  inline 
  Vector2d operator+ (const Vector2d& vec) const  {
    return Vector2d(x + vec.x, y + vec.y);
  }

  inline 
  Vector2d operator- () const  {
    return Vector2d(-x, -y);
  }

  inline 
  Vector2d operator- (const Vector2d& vec) const  {
    return Vector2d(x - vec.x, y - vec.y);
  }

  inline 
  Vector2d operator* (float f) const  {
    return Vector2d(x * f, y * f);
  }

  inline 
  float norm() const {
    return sqrt (x*x + y*y);
  }


  inline 
  void normalize() {
    float f = norm();
    if (f!=0)
      {
        x /= f;
        y /= f;
      }
  }

};

inline 
std::ostream& operator << (std::ostream& os, const Vector2d& v)
{
  return os << "[" << v.x << ", " << v.y << "]";
}

#endif

/* EOF */
