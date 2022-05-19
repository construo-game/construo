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

#include "zoom_state.hpp"

GCZoomState::GCZoomState(geom::frect const& rect) :
  m_bounding_box(rect),
  m_x_offset(0.0f),
  m_y_offset(0.0f),
  m_scale(1.0f)
{
}

bool
GCZoomState::zoom_in(geom::fpoint const& screen_pos)
{
  geom::fpoint const pos = screen_to_world(screen_pos);

  if ((true))
  {
    float old_zoom = m_scale;
    set_zoom(m_scale * 1.2f);
    m_x_offset = screen_pos.x() / m_scale - screen_pos.x() / old_zoom + m_x_offset;
    m_y_offset = screen_pos.y() / m_scale - screen_pos.y() / old_zoom + m_y_offset;

  }
  else
  {
    m_x_offset = (pos.x() + m_x_offset) / 1.2f - pos.x();
    m_y_offset = (pos.y() + m_y_offset) / 1.2f - pos.y();
    m_scale *= 1.2f;
  }

  return true;
}

bool
GCZoomState::zoom_out(geom::fpoint const& screen_pos)
{
  geom::fpoint const pos = screen_to_world(screen_pos);

  if ((true))
  {
    float const old_zoom = m_scale;
    set_zoom(m_scale / 1.2f);
    m_x_offset = screen_pos.x() / m_scale - screen_pos.x() / old_zoom + m_x_offset;
    m_y_offset = screen_pos.y() / m_scale - screen_pos.y() / old_zoom + m_y_offset;
  }
  else
  {
    m_x_offset = (pos.x() + m_x_offset) * 1.2f - pos.x();
    m_y_offset = (pos.y() + m_y_offset) * 1.2f - pos.y();

    m_scale *= (1.0f/1.2f);
  }

  return true;
}

geom::fpoint
GCZoomState::screen_to_world(geom::fpoint const& pos) const
{
  return geom::fpoint((pos.x() / m_scale) - m_x_offset,
                      (pos.y() / m_scale) - m_y_offset);
}

geom::fpoint
GCZoomState::world_to_screen(geom::fpoint const& pos) const
{
  return geom::fpoint((pos.x() + m_x_offset) * m_scale + m_bounding_box.left(),
                      (pos.y() + m_y_offset) * m_scale + m_bounding_box.top());
}

float
GCZoomState::screen_to_world_x(float x) const
{
  return (x / m_scale) - m_x_offset;
}

float
GCZoomState::screen_to_world_y(float y) const
{
  return (y / m_scale) - m_y_offset;
}

void
GCZoomState::move_to(geom::fpoint const& pos)
{
  m_x_offset = (m_bounding_box.width()  / (2 * m_scale)) + pos.x();
  m_y_offset = (m_bounding_box.height() / (2 * m_scale)) + pos.y();
}

void
GCZoomState::translate(geom::foffset const& offset)
{
  m_x_offset -= offset.x();
  m_y_offset -= offset.y();
}

void
GCZoomState::set_offset(geom::foffset const& offset)
{
  m_x_offset = offset.x();
  m_y_offset = offset.y();
}

bool
GCZoomState::set_zoom(const float& z)
{
  const float max_zoom = 20.0f;
  const float min_zoom = 0.05f;

  if (z > max_zoom)
  {
    m_scale = max_zoom;
    return false;
  }
  else if (z < min_zoom)
  {
    m_scale = min_zoom;
    return false;
  }
  else
  {
    m_scale = z;
    return true;
  }
}

void
GCZoomState::zoom_to(geom::frect const& rect)
{
  float const center_x = (rect.left() + rect.right()) / 2.0f;
  float const center_y = (rect.top() + rect.bottom()) / 2.0f;

  float const width  = rect.right() - rect.left();
  float const height = rect.bottom() - rect.top();
  float const screen_relation = m_bounding_box.height() / m_bounding_box.width();
  float const rect_relation   = height/width;

  if (rect_relation < screen_relation) // take width, ignore height
  {
    set_zoom(m_bounding_box.width() / width);
  }
  else // take height, ignore width
  {
    set_zoom(m_bounding_box.height()/height);
  }

  m_x_offset = (m_bounding_box.width()  / (2 * m_scale)) - center_x;
  m_y_offset = (m_bounding_box.height() / (2 * m_scale)) - center_y;
}

/* EOF */
