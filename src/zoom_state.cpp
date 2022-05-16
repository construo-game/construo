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
  m_x1(rect.left()),
  m_y1(rect.top()),
  m_x2(rect.right()),
  m_y2(rect.bottom()),
  m_x_offset(0.0f),
  m_y_offset(0.0f),
  m_scale(1.0f)
{
}

bool
GCZoomState::zoom_in(float screen_x, float screen_y)
{
  float const x = screen_to_world_x(screen_x);
  float const y = screen_to_world_y(screen_y);

  if ((true))
  {
    float old_zoom = m_scale;
    set_zoom(m_scale * 1.2f);
    m_x_offset = screen_x / m_scale - screen_x/old_zoom + m_x_offset;
    m_y_offset = screen_y / m_scale - screen_y/old_zoom + m_y_offset;

  }
  else
  {
    m_x_offset = (x + m_x_offset)/1.2f - x;
    m_y_offset = (y + m_y_offset)/1.2f - y;
    m_scale *= 1.2f;
  }

  return true;
}

bool
GCZoomState::zoom_out(float screen_x, float screen_y)
{
  float const x = screen_to_world_x(screen_x);
  float const y = screen_to_world_y(screen_y);

  if ((true))
  {
    float const old_zoom = m_scale;
    set_zoom(m_scale / 1.2f);
    m_x_offset = screen_x / m_scale - screen_x/old_zoom + m_x_offset;
    m_y_offset = screen_y / m_scale - screen_y/old_zoom + m_y_offset;
  }
  else
  {
    m_x_offset = (x + m_x_offset) * 1.2f - x;
    m_y_offset = (y + m_y_offset) * 1.2f - y;

    m_scale *= (1.0f/1.2f);
  }

  return true;
}

glm::vec2
GCZoomState::screen_to_world(const glm::vec2& pos) const
{
  return glm::vec2((pos.x / m_scale) - m_x_offset,
                  (pos.y / m_scale) - m_y_offset);
}

glm::vec2
GCZoomState::world_to_screen(const glm::vec2& pos) const
{
  return glm::vec2((pos.x + m_x_offset) * m_scale + static_cast<float>(m_x1),
                  (pos.y + m_y_offset) * m_scale + m_y1);
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

float
GCZoomState::world_to_screen_x(float x) const
{
  return (x + m_x_offset) * m_scale + static_cast<float>(m_x1);
}

float
GCZoomState::world_to_screen_y(float y) const
{
  return (y + m_y_offset) * m_scale + m_y1;
}

void
GCZoomState::move_to(float x, float y)
{
  m_x_offset = (bounding_width()  / (2 * m_scale)) + x;
  m_y_offset = (bounding_height() / (2 * m_scale)) + y;
}

void
GCZoomState::translate_offset(float x, float y)
{
  m_x_offset -= x;
  m_y_offset -= y;
}

void
GCZoomState::set_offset(float x, float y)
{
  m_x_offset = x;
  m_y_offset = y;
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
GCZoomState::zoom_to(float x1, float y1, float x2, float y2)
{
  float const center_x = (x1 + x2) / 2.0f;
  float const center_y = (y1 + y2) / 2.0f;

  float const width  = x2 - x1;
  float const height = y2 - y1;
  float const screen_relation = bounding_height() / bounding_width();
  float const rect_relation   = height/width;

  if (rect_relation < screen_relation) // take width, ignore height
  {
    set_zoom(bounding_width()/width);
  }
  else // take height, ignore width
  {
    set_zoom(bounding_height()/height);
  }

  m_x_offset = (bounding_width()  / (2 * m_scale)) - center_x;
  m_y_offset = (bounding_height() / (2 * m_scale)) - center_y;
}

/* EOF */
