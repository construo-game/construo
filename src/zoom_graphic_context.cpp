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
ZoomGraphicContext::set_clip_rect(geom::frect const& rect)
{
  m_parent_gc.set_clip_rect(rect);
}

void
ZoomGraphicContext::clear_clip_rect()
{
  m_parent_gc.clear_clip_rect();
}

void
ZoomGraphicContext::lock()
{
  m_parent_gc.set_clip_rect(m_zoom.bounding_box());
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
    i->p1 = m_zoom.world_to_screen(i->p1);
    i->p2 = m_zoom.world_to_screen(i->p2);
  }
  m_parent_gc.draw_lines(lines, color, wide);
}

void
ZoomGraphicContext::draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide)
{
  m_parent_gc.draw_line(m_zoom.world_to_screen(p1),
                        m_zoom.world_to_screen(p2),
                        color, wide);
}

void
ZoomGraphicContext::draw_rect(geom::frect const& rect, Color color)
{
  m_parent_gc.draw_rect(geom::frect(m_zoom.world_to_screen(rect.topleft()),
                                    m_zoom.world_to_screen(rect.bottomright())),
                        color);
}

void
ZoomGraphicContext::draw_circles(std::vector<Circle>& circles, Color color)
{
  for (auto i = circles.begin(); i != circles.end(); ++i)
  {
    i->pos = m_zoom.world_to_screen(i->pos);
    i->r = std::max(2.0f, i->r * m_zoom.get_scale());
  }

  m_parent_gc.draw_circles(circles, color);
}

void
ZoomGraphicContext::draw_circle(geom::fpoint const& pos, float r, Color color)
{
  m_parent_gc.draw_circle(m_zoom.world_to_screen(pos),
                          std::max(2.0f, r * m_zoom.get_scale()),
                          color);
}

void
ZoomGraphicContext::draw_fill_circle(geom::fpoint const& pos, float r, Color color)
{
  m_parent_gc.draw_fill_circle(m_zoom.world_to_screen(pos),
                               std::max(2.0f, r * m_zoom.get_scale()),
                               color);
}

void
ZoomGraphicContext::draw_fill_rect(geom::frect const& rect, Color color)
{
  m_parent_gc.draw_fill_rect(geom::frect(m_zoom.world_to_screen(rect.topleft()),
                                         m_zoom.world_to_screen(rect.bottomright())),
                             color);
}

void
ZoomGraphicContext::draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color)
{
  m_parent_gc.draw_string_centered(m_zoom.world_to_screen(pos), str, color);
}

void
ZoomGraphicContext::draw_string(geom::fpoint const& pos, const std::string& str, Color color)
{
  m_parent_gc.draw_string(m_zoom.world_to_screen(pos), str, color);
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
