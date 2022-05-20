// Construo - A wire-frame construction game
// Copyright (C) 2002-2022 Ingo Ruhnke <grumbel@gmail.com>
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

#ifndef HEADER_ZOOM_STATE_HPP
#define HEADER_ZOOM_STATE_HPP

#include <glm/glm.hpp>
#include <geom/rect.hpp>

namespace construo {

class ZoomState
{
public:
  ZoomState(geom::frect const& rect);

  /**
   * Zoom Out the GraphicContext by one 'unit', using the x and y
   * position as center.
   *
   * @param x x in screen coordinates
   * @param y y in screen coordinates
   *
   * @return true if zoom was successfull, false if zoom boundaries
   * have been reached
   */
  bool zoom_in(geom::fpoint const& pos);

  /**
   * Zoom Into the GraphicContext by one 'unit', using the x and y
   * position as center.
   *
   * @param x x in screen coordinates
   * @param y y in screen coordinates
   *
   * @return true if zoom was successfull, false if zoom boundaries
   * have been reached
   */
  bool zoom_out(geom::fpoint const& pos);

  /**
   * Zoom into the given area, all coordinates in world units
   *
   * @param x1 left border in world coordinates
   * @param y1 top border in world coordinates
   * @param x2 right border in world coordinates
   * @param y2 bottom border in world coordinates
   */
  void zoom_to(geom::frect const& rect);

  /** Move to the given x and y world coordinates to the center of the
      screen */
  void move_to(geom::fpoint const& pos);

  /** Move the current position relativly x/y width */
  void translate(geom::foffset const& offset);

 /** FIXME: What exactly is an offset?! */
  void set_offset(geom::foffset const& offset);

  geom::foffset get_offset() const { return m_offset; }

  /** @return current zoom factor in use */
  float get_scale() const { return m_scale; }

  /** Set the zoom factor, if zoom is to large/small false will
      returned and zoom will be set to lowest/highest possible
      value */
  bool  set_zoom(const float& z);

  /** Convert a coordinate from screen units to world units */
  geom::fpoint screen_to_world(geom::fpoint const& pos) const;

  /** Convert a coordinate from world units to screen units */
  geom::fpoint world_to_screen(geom::fpoint const& pos) const;

  geom::frect bounding_box() const { return m_bounding_box; }

  void set_bounding_box(geom::frect const& rect) {
    m_bounding_box = rect;
  }

private:
  geom::frect m_bounding_box;
  geom::foffset m_offset;
  float m_scale;
};

} // namespace construo

#endif

/* EOF */
