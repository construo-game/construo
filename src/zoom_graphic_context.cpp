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

#include "zoom_graphic_context.hpp"

#include <assert.h>

#include "construo.hpp"
#include "math.hpp"
#include "root_graphic_context.hpp"

ZoomGraphicContext::ZoomGraphicContext(GraphicContext& gc, GCZoomState& zoom) :
  m_parent_gc(gc),
  m_zoom(zoom)
{
}

ZoomGraphicContext::~ZoomGraphicContext()
{
}

void
ZoomGraphicContext::set_clip_rect(float x1_, float y1_, float x2_, float y2_)
{
  m_parent_gc.set_clip_rect(x1_, y1_, x2_, y2_);
}

void
ZoomGraphicContext::clear_clip_rect()
{
  m_parent_gc.clear_clip_rect();
}

void
ZoomGraphicContext::lock()
{
  m_parent_gc.set_clip_rect(m_zoom.bounding_x1(), m_zoom.bounding_y1(),
                            m_zoom.bounding_x2(), m_zoom.bounding_y2());
}

void
ZoomGraphicContext::unlock()
{
  m_parent_gc.clear_clip_rect();
}

void
ZoomGraphicContext::draw_lines(std::vector<Line>& lines, Color color, int wide)
{
  for (auto i = lines.begin(); i != lines.end(); ++i)
  {
    i->x1 = m_zoom.world_to_screen_x(i->x1);
    i->y1 = m_zoom.world_to_screen_y(i->y1);
    i->x2 = m_zoom.world_to_screen_x(i->x2);
    i->y2 = m_zoom.world_to_screen_y(i->y2);
  }
  m_parent_gc.draw_lines(lines, color, wide);
}

void
ZoomGraphicContext::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  m_parent_gc.draw_line(m_zoom.world_to_screen_x(x1),
                        m_zoom.world_to_screen_y(y1),
                        m_zoom.world_to_screen_x(x2),
                        m_zoom.world_to_screen_y(y2),
                        color, wide);
}

void
ZoomGraphicContext::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  m_parent_gc.draw_rect(m_zoom.world_to_screen_x(x1),
                        m_zoom.world_to_screen_y(y1),
                        m_zoom.world_to_screen_x(x2),
                        m_zoom.world_to_screen_y(y2),
                        color);
}

void
ZoomGraphicContext::draw_circles(std::vector<Circle>& circles, Color color)
{
  for (auto i = circles.begin(); i != circles.end(); ++i)
  {
    i->x = m_zoom.world_to_screen_x(i->x);
    i->y = m_zoom.world_to_screen_x(i->y);
    i->r = std::max(2.0f, i->r * m_zoom.get_scale());
  }

  m_parent_gc.draw_circles(circles, color);
}

void
ZoomGraphicContext::draw_circle(float x, float y, float r, Color color)
{
  m_parent_gc.draw_circle(m_zoom.world_to_screen_x(x),
                          m_zoom.world_to_screen_y(y),
                          std::max(2.0f, r * m_zoom.get_scale()),
                          color);
}

void
ZoomGraphicContext::draw_fill_circle(float x, float y, float r, Color color)
{
  m_parent_gc.draw_fill_circle(m_zoom.world_to_screen_x(x),
                               m_zoom.world_to_screen_y(y),
                               std::max(2.0f, r * m_zoom.get_scale()),
                               color);
}

void
ZoomGraphicContext::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  m_parent_gc.draw_fill_rect(m_zoom.world_to_screen_x(x1),
                             m_zoom.world_to_screen_y(y1),
                             m_zoom.world_to_screen_x(x2),
                             m_zoom.world_to_screen_y(y2),
                             color);
}

void
ZoomGraphicContext::draw_string_centered(float x, float y, const std::string& str, Color color)
{
  m_parent_gc.draw_string_centered(m_zoom.world_to_screen_x(x),
                                   m_zoom.world_to_screen_y(y),
                                   str,
                                   color);
}

void
ZoomGraphicContext::draw_string(float x, float y, const std::string& str, Color color)
{
  m_parent_gc.draw_string(m_zoom.world_to_screen_x(x),
                          m_zoom.world_to_screen_y(y),
                          str,
                          color);
}

float
ZoomGraphicContext::get_width()
{
  return m_zoom.bounding_width();
}

float
ZoomGraphicContext::get_height()
{
  return m_zoom.bounding_height();
}

/* EOF */
