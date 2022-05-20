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

#ifndef HEADER_CONSTRUO_GRAPHIC_CONTEXT_HPP
#define HEADER_CONSTRUO_GRAPHIC_CONTEXT_HPP

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <geom/point.hpp>
#include <geom/rect.hpp>

#include "math.hpp"
#include "color.hpp"

/** Graphic abstraction interface */
namespace construo {

class GraphicContext
{
public:
  struct Line
  {
    geom::fpoint p1;
    geom::fpoint p2;
  };

  struct Circle
  {
    geom::fpoint pos;
    float r;
  };

public:
  virtual ~GraphicContext() {}

  virtual void draw_lines(std::vector<Line>& lines, Color color, int wide = 0) = 0;
  virtual void draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide = 0) = 0;
  virtual void draw_rect(geom::frect const& rect, Color color) = 0;
  virtual void draw_circle(geom::fpoint const& pos, float radius, Color color) = 0;
  virtual void draw_circles(std::vector<Circle>& circles, Color color) = 0;
  virtual void draw_fill_circle(geom::fpoint const& pos, float radius, Color color) = 0;
  virtual void draw_fill_rect(geom::frect const& rect, Color color) = 0;
  virtual void draw_string(geom::fpoint const& pos, const std::string& str, Color color = Color(0xFFFFFFFF)) = 0;
  virtual void draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color = Color(0xFFFFFFFF)) = 0;

  virtual void set_clip_rect(geom::frect const& rect) = 0;
  virtual void clear_clip_rect() = 0;

  /** Return the window size and position */
  virtual geom::frect geometry() const = 0;

  virtual void clear() = 0;

  /** FIXME: flip should be handled outsite of GraphicContext */
  virtual void flip() = 0;

  /** Goes into quick draw mode, disabling anti-aliasing and other
      time consuming features */
  virtual void push_quick_draw() {}

  /** Goes out of quickdraw mode, restoring previous settings */
  virtual void pop_quick_draw() {}
};

} // namespace construo

#endif

/* EOF */
