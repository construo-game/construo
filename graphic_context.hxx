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

#ifndef HEADER_CONSTRUO_GRAPHIC_CONTEXT_HXX
#define HEADER_CONSTRUO_GRAPHIC_CONTEXT_HXX

#include <string>
#include "color.hxx"

/** Graphic abstraction interface */
class GraphicContext
{
private:
public:
  virtual void draw_line(int x1, int y1, int x2, int y2, Color color, int wide = 0) =0;
  virtual void draw_rect(int x1, int y1, int x2, int y2, Color color) =0;
  virtual void draw_circle(int x, int y, int r, Color color) =0;
  virtual void draw_fill_circle(int x, int y, int r, Color color) =0;
  virtual void draw_fill_rect(int x1, int y1, int x2, int y2, Color color) =0;
  virtual void draw_string(int x, int y, const std::string& str, Color color = Color (0xFFFFFF)) =0;
  virtual void clear () =0;
  virtual void flip () =0;
};

#endif

/* EOF */
