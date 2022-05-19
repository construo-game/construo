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

#ifndef HEADER_ZOOM_GRAPHIC_CONTEXT_HPP
#define HEADER_ZOOM_GRAPHIC_CONTEXT_HPP

#include <glm/glm.hpp>

#include "graphic_context.hpp"
#include "zoom_state.hpp"

/** A GraphicContext, that wraps another GraphicContext and adds
    coordinate translation like zoom and scrolling */
class ZoomGraphicContext : public GraphicContext
{
public:
  ZoomGraphicContext(GraphicContext& gc, GCZoomState& zoom);
  virtual ~ZoomGraphicContext();

  void set_clip_rect(geom::frect const& rect) override;
  void clear_clip_rect() override;

  void draw_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_circles(std::vector<Circle>& circles, Color color) override;
  void draw_fill_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_fill_rect(geom::frect const& rect, Color color) override;
  void draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide = 0) override;
  void draw_lines(std::vector<Line>& lines, Color color, int wide = 0) override;
  void draw_rect(geom::frect const& rect, Color color) override;
  void draw_string(geom::fpoint const& pos, const std::string& str, Color color = Color(0xFFFFFFFF)) override;
  void draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color = Color(0xFFFFFFFF)) override;

  geom::frect geometry() const override;

  void clear() override { m_parent_gc.clear(); }

  /** FIXME: flip should be handled outsite of GraphicContext */
  void flip() override { m_parent_gc.flip(); }

  void push_quick_draw() override { m_parent_gc.push_quick_draw(); }
  void pop_quick_draw() override { m_parent_gc.pop_quick_draw(); }

  /** Sets the clipping rectangles needed for the GC */
  void lock();

  /** Resets the clipping to the old state */
  void unlock();

  /** Allows a client application to make use of the partent GC in
      case it wants to draw elements which should not get scaled */
  GraphicContext& get_parent_gc() { return m_parent_gc; }

  GCZoomState const& zoom() const { return m_zoom; }

private:
  float translate_x(float x);
  float translate_y(float y);

private:
  GraphicContext& m_parent_gc;
  GCZoomState& m_zoom;

public:
  ZoomGraphicContext(const ZoomGraphicContext&) = delete;
  ZoomGraphicContext& operator=(const ZoomGraphicContext&) = delete;
};

#endif

/* EOF */
