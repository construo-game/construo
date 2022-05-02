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

#include "vector2d.hpp"

// FIXME: ZoomState name conflicts with X11
class GCZoomState
{
public:
  GCZoomState(float x1, float y1, float x2, float y2);

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
  bool zoom_in(float x, float y);

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
  bool zoom_out(float x, float y);

  /**
   * Zoom into the given area, all coordinates in world units
   *
   * @param x1 left border in world coordinates
   * @param y1 top border in world coordinates
   * @param x2 right border in world coordinates
   * @param y2 bottom border in world coordinates
   */
  void zoom_to(float x1, float y1, float x2, float y2);

  /** Move to the given x and y world coordinates to the center of the
      screen */
  void move_to(float x, float y);

  /** Move the current position relativly x/y width */
  void translate_offset(float x, float y);

 /** FIXME: What exactly is an offset?! */
  void set_offset(float x, float y);

  /** FIXME: What exactly is an offset?! */
  float get_x_offset() { return m_x_offset; }

  /** FIXME: What exactly is an offset?! */
  float get_y_offset() { return m_y_offset; }

  /** @return current zoom factor in use */
  float get_zoom() const { return m_zoom; }

  /** Set the zoom factor, if zoom is to large/small false will
      returned and zoom will be set to lowest/highest possible
      value */
  bool  set_zoom(const float& z);

  /** Convert a coordinate from screen units to world units */
  float screen_to_world_x(float x) const;

  /** Convert a coordinate from screen units to world units */
  float screen_to_world_y(float y) const;

  /** Convert a coordinate from world units to screen units */
  float world_to_screen_x(float x) const;

  /** Convert a coordinate from world units to screen units */
  float world_to_screen_y(float y) const;

  /** Convert a coordinate from screen units to world units */
  Vector2d screen_to_world(const Vector2d&) const;

  /** Convert a coordinate from world units to screen units */
  Vector2d world_to_screen(const Vector2d&) const;

  float bounding_x1() const { return m_x1; }
  float bounding_y1() const { return m_y1; }
  float bounding_x2() const { return m_x2; }
  float bounding_y2() const { return m_y2; }
  float bounding_width() const { return m_x2 - m_x1; }
  float bounding_height() const { return m_y2 - m_y1; }

  void set_bounding_box(float x, float y, float width, float height) {
    m_x1 = x;
    m_y1 = y;
    m_x2 = x + width;
    m_y2 = y + height;
  }

private:
  // bounding box
  float m_x1;
  float m_y1;
  float m_x2;
  float m_y2;

  // zoom state
  float m_x_offset;
  float m_y_offset;
  float m_zoom;
};

#endif

/* EOF */
