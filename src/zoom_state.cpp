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

GCZoomState::GCZoomState(float x1, float y1, float x2, float y2) :
  m_x1(x1),
  m_y1(y1),
  m_x2(x2),
  m_y2(y2),
  m_x_offset(0.0f),
  m_y_offset(0.0f),
  m_zoom(1.0f)
{
}

bool
GCZoomState::zoom_in(float screen_x, float screen_y)
{
  float const x = screen_to_world_x(screen_x);
  float const y = screen_to_world_y(screen_y);

  //std::cout << "Zoom: " << screen_x << " " << screen_y
  //<< " " << x << " " << y << std::endl;

  if ((true))
  {
    float old_zoom = m_zoom;
    set_zoom(m_zoom * 1.2f);
    m_x_offset = screen_x / m_zoom - screen_x/old_zoom + m_x_offset;
    m_y_offset = screen_y / m_zoom - screen_y/old_zoom + m_y_offset;

  }
  else
  {
    m_x_offset = (x + m_x_offset)/1.2f - x;
    m_y_offset = (y + m_y_offset)/1.2f - y;
    m_zoom *= 1.2f;
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
    float const old_zoom = m_zoom;
    set_zoom(m_zoom / 1.2f);
    m_x_offset = screen_x / m_zoom - screen_x/old_zoom + m_x_offset;
    m_y_offset = screen_y / m_zoom - screen_y/old_zoom + m_y_offset;
  }
  else
  {
    m_x_offset = (x + m_x_offset) * 1.2f - x;
    m_y_offset = (y + m_y_offset) * 1.2f - y;

    m_zoom *= (1.0f/1.2f);
  }

  return true;
}

Vector2d
GCZoomState::screen_to_world(const Vector2d& pos) const
{
  return Vector2d((pos.x / m_zoom) - m_x_offset,
                  (pos.y / m_zoom) - m_y_offset);
}

Vector2d
GCZoomState::world_to_screen(const Vector2d& pos) const
{
  return Vector2d((pos.x + m_x_offset) * m_zoom + static_cast<float>(m_x1),
                  (pos.y + m_y_offset) * m_zoom + m_y1);
}

float
GCZoomState::screen_to_world_x(float x) const
{
  return (x / m_zoom) - m_x_offset;
}

float
GCZoomState::screen_to_world_y(float y) const
{
  return (y / m_zoom) - m_y_offset;
}

float
GCZoomState::world_to_screen_x(float x) const
{
  return (x + m_x_offset) * m_zoom + static_cast<float>(m_x1);
}

float
GCZoomState::world_to_screen_y(float y) const
{
  return (y + m_y_offset) * m_zoom + m_y1;
}

void
GCZoomState::move_to(float x, float y)
{
  m_x_offset = (bounding_width()  / (2 * m_zoom)) + x;
  m_y_offset = (bounding_height() / (2 * m_zoom)) + y;
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
    m_zoom = max_zoom;
    return false;
  }
  else if (z < min_zoom)
  {
    m_zoom = min_zoom;
    return false;
  }
  else
  {
    m_zoom = z;
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

  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
  {
    set_zoom(bounding_width()/width);
  }
  else // take height, ignore width
  {
    set_zoom(bounding_height()/height);
  }

  m_x_offset = (bounding_width()  / (2 * m_zoom)) - center_x;
  m_y_offset = (bounding_height() / (2 * m_zoom)) - center_y;
}

/* EOF */
