//  $Id$
// 
//  Construo - A wire-frame construction game
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

#ifndef HEADER_CONSTRUO_COLOR_HXX
#define HEADER_CONSTRUO_COLOR_HXX

/** */
struct Color
{
  float r;
  float g;
  float b;
  float a;

  Color (int i) {
    r = ((i & 0xFF000000) >> 24) / 255.0f;
    g = ((i & 0x00FF0000) >> 16) / 255.0f;
    b = ((i & 0x0000FF00) >>  8) / 255.0f;
    a = ((i & 0x000000FF) >>  0) / 255.0f;
  }
  
  Color (float r, float g, float b, float a = 1.0f)
  : r (r), g (g), b (b), a (a) {}

  /** Convert the color into 0xRRGGBB format */
  inline unsigned int get_rgb () { return int(255 * b) | (int((255 * g)) << 8) | (int((255 * r)) << 16); }
};

#endif

/* EOF */
