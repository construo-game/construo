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

#ifndef HEADER_CONSTRUO_GRAPHIC_CONTEXT_HPP
#define HEADER_CONSTRUO_GRAPHIC_CONTEXT_HPP

#include <string>
#include <vector>
#include "math.hpp"
#include "vector2d.hpp"
#include "color.hpp"

/** Graphic abstraction interface */
class GraphicContext
{
private:
public:
  struct Line
  {
    float x1, y1;
    float x2, y2;
  };

  struct Circle
  {
    float x, y, r;
  };

  void draw_circle(const Vector2d& pos, float radius, Color color)
  {
    draw_circle (pos.x, pos.y, radius, color);
  }

  void draw_fill_circle(const Vector2d& pos, float radius, Color color)
  {
    draw_fill_circle (pos.x, pos.y, radius, color);
  }

  void draw_string(const Vector2d& pos, const std::string& str, Color color = Color (0xFFFFFFFF))
  {
    draw_string (pos.x, pos.y, str, color);
  }

  void draw_line (const Vector2d& pos1, const Vector2d& pos2, Color color, int wide = 0)
  {
    draw_line (pos1.x, pos1.y, pos2.x, pos2.y, color, wide);
  }

  void draw_rect (const Vector2d& pos1, const Vector2d& pos2, Color color)
  {
    draw_rect (Math::min(pos1.x, pos2.x),
               Math::min(pos1.y, pos2.y),
               Math::max(pos1.x, pos2.x),
               Math::max(pos1.y, pos2.y),
               color);
  }

  void draw_fill_rect (const Vector2d& pos1, const Vector2d& pos2, Color color)
  {
    draw_fill_rect (pos1.x, pos1.y, pos2.x, pos2.y, color);
  }

  virtual void draw_lines (std::vector<Line>& lines, Color color, int wide = 0) =0;
  virtual void draw_line(float x1, float y1, float x2, float y2, Color color, int wide = 0) =0;
  virtual void draw_rect(float x1, float y1, float x2, float y2, Color color) =0;
  virtual void draw_circle(float x, float y, float radius, Color color) =0;
  virtual void draw_circles(std::vector<Circle>& circles, Color color) =0;
  virtual void draw_fill_circle(float x, float y, float radius, Color color) =0;
  virtual void draw_fill_rect(float x1, float y1, float x2, float y2, Color color) =0;
  virtual void draw_string(float x, float y, const std::string& str, Color color = Color (0xFFFFFFFF)) =0;

  virtual void draw_string_centered(float x, float y, const std::string& str, Color color = Color (0xFFFFFFFF)) =0;

  virtual void set_clip_rect (int x1_, int y1_, int x2_, int y2_) =0;

  virtual int get_width ()  =0;
  virtual int get_height () =0;

  virtual void clear () =0;

  /** FIXME: flip should be handled outsite of GraphicContext */
  virtual void flip () =0;
  virtual void real_flip () {}
  virtual void flip (int x1, int y1, int x2, int y2) =0;

  /** Goes into quick draw mode, disabling anti-aliasing and other
      time consuming features */
  virtual void push_quick_draw() {}

  /** Goes out of quickdraw mode, restoring previous settings */
  virtual void pop_quick_draw() {}
};

#endif

/* EOF */
