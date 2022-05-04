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

#ifndef HEADER_CONSTRUO_COLOR_HPP
#define HEADER_CONSTRUO_COLOR_HPP

#include <stdint.h>

struct Color
{
  float r;
  float g;
  float b;
  float a;

  Color() :
    r(0.0f), g(0.0f), b(0.0f), a(0.0f)
  {}

  Color (uint32_t i) :
    r(static_cast<float>((i & 0xFF000000) >> 24) / 255.0f),
    g(static_cast<float>((i & 0x00FF0000) >> 16) / 255.0f),
    b(static_cast<float>((i & 0x0000FF00) >>  8) / 255.0f),
    a(static_cast<float>((i & 0x000000FF) >>  0) / 255.0f)
  {}

  Color (float r_, float g_, float b_, float a_ = 1.0f) :
    r(r_), g (g_), b (b_), a (a_) {}

  /** Convert the color into 0xRRGGBB format */
  inline unsigned int get_as_rrggbb () const
  {
    return (static_cast<unsigned int>(255.0f * b))
      |   ((static_cast<unsigned int>(255.0f * g)) << 8)
      |   ((static_cast<unsigned int>(255.0f * r)) << 16);
  }
};

#endif

/* EOF */
