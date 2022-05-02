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

#include <iostream>
#include <assert.h>
#include "construo.hpp"
#include "math.hpp"
#include "root_graphic_context.hpp"
#include "zoom_graphic_context.hpp"

ZoomGraphicContext::ZoomGraphicContext() :
  m_x1(0.0f),
  m_y1(0.0f),
  // FIXME: should use parent gc
  m_x2(g_graphic_context->get_width()),
  m_y2(g_graphic_context->get_height()),
  m_x_offset(0.0f),
  m_y_offset(0.0f),
  m_zoom(1.0f),
  m_parent_gc(nullptr)
{
}

ZoomGraphicContext::ZoomGraphicContext(float x1_, float y1_, float x2_, float y2_) :
  m_x1(x1_),
  m_y1(y1_),
  m_x2(x2_),
  m_y2(y2_),
  m_x_offset(0.0f),
  m_y_offset(0.0f),
  m_zoom(1.0f),
  m_parent_gc(nullptr)
{
}

ZoomGraphicContext::~ZoomGraphicContext()
{
}

void
ZoomGraphicContext::set_clip_rect (float x1_, float y1_, float x2_, float y2_)
{
  m_parent_gc->set_clip_rect(x1_, y1_, x2_, y2_);
}

void
ZoomGraphicContext::lock ()
{
  m_parent_gc->set_clip_rect(m_x1, m_y1, m_x2, m_y2);
}

void
ZoomGraphicContext::unlock ()
{
  m_parent_gc->set_clip_rect(0, 0, m_parent_gc->get_width ()-1, m_parent_gc->get_height()-1);
}

Vector2d
ZoomGraphicContext::screen_to_world (const Vector2d& pos)
{
  return Vector2d ((pos.x / m_zoom) - m_x_offset,
                   (pos.y / m_zoom) - m_y_offset);
}

Vector2d
ZoomGraphicContext::world_to_screen (const Vector2d& pos)
{
  return Vector2d ((pos.x + m_x_offset) * m_zoom + static_cast<float>(m_x1),
                   (pos.y + m_y_offset) * m_zoom + m_y1);
}

float
ZoomGraphicContext::screen_to_world_x (float x)
{
  return (x / m_zoom) - m_x_offset;
}

float
ZoomGraphicContext::screen_to_world_y (float y)
{
  return (y / m_zoom) - m_y_offset;
}

float
ZoomGraphicContext::world_to_screen_x (float x)
{
  return (x + m_x_offset) * m_zoom + static_cast<float>(m_x1);
}

float
ZoomGraphicContext::world_to_screen_y (float y)
{
  return (y + m_y_offset) * m_zoom + m_y1;
}

void
ZoomGraphicContext::draw_lines (std::vector<Line>& lines, Color color, int wide)
{
  for (std::vector<Line>::iterator i = lines.begin(); i != lines.end(); ++i)
  {
    i->x1 = world_to_screen_x(i->x1);
    i->y1 = world_to_screen_y(i->y1);
    i->x2 = world_to_screen_x(i->x2);
    i->y2 = world_to_screen_y(i->y2);
  }
  m_parent_gc->draw_lines(lines, color, wide);
}

void
ZoomGraphicContext::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  m_parent_gc->draw_line(world_to_screen_x(x1),
                         world_to_screen_y(y1),
                         world_to_screen_x(x2),
                         world_to_screen_y(y2),
                         color, wide);
}

void
ZoomGraphicContext::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  m_parent_gc->draw_rect(world_to_screen_x(x1),
                         world_to_screen_y(y1),
                         world_to_screen_x(x2),
                         world_to_screen_y(y2),
                         color);
}

void
ZoomGraphicContext::draw_circles(std::vector<Circle>& circles, Color color)
{
  for (std::vector<Circle>::iterator i = circles.begin(); i != circles.end(); ++i)
  {
    i->x = world_to_screen_x(i->x);
    i->y = world_to_screen_x(i->y);
    i->r = Math::max(2.0f, i->r * m_zoom);
  }

  m_parent_gc->draw_circles(circles, color);
}

void
ZoomGraphicContext::draw_circle(float x, float y, float r, Color color)
{
  m_parent_gc->draw_circle(world_to_screen_x(x),
                           world_to_screen_y(y),
                           Math::max(2.0f, r * m_zoom),
                           color);
}

void
ZoomGraphicContext::draw_fill_circle(float x, float y, float r, Color color)
{
  m_parent_gc->draw_fill_circle(world_to_screen_x(x),
                                world_to_screen_y(y),
                                Math::max(2.0f, r * m_zoom),
                                color);
}

void
ZoomGraphicContext::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  m_parent_gc->draw_fill_rect(world_to_screen_x(x1),
                              world_to_screen_y(y1),
                              world_to_screen_x(x2),
                              world_to_screen_y(y2),
                              color);
}

void
ZoomGraphicContext::draw_string_centered(float x, float y, const std::string& str, Color color)
{
  m_parent_gc->draw_string_centered(world_to_screen_x(x),
                                    world_to_screen_y(y),
                                    str,
                                    color);
}

void
ZoomGraphicContext::draw_string(float x, float y, const std::string& str, Color color)
{
  m_parent_gc->draw_string(world_to_screen_x(x),
                           world_to_screen_y(y),
                           str,
                           color);
}

void
ZoomGraphicContext::set_parent_gc (GraphicContext* gc)
{
  m_parent_gc = gc;
}

bool
ZoomGraphicContext::zoom_in(float screen_x, float screen_y)
{
  float const x = screen_to_world_x (screen_x);
  float const y = screen_to_world_y (screen_y);

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
ZoomGraphicContext::zoom_out (float screen_x, float screen_y)
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

void
ZoomGraphicContext::move_to (float x, float y)
{
  m_x_offset = (get_width()  / (2 * m_zoom)) + x;
  m_y_offset = (get_height() / (2 * m_zoom)) + y;
}

void
ZoomGraphicContext::translate_offset (float x, float y)
{
  m_x_offset -= x;
  m_y_offset -= y;
}

void
ZoomGraphicContext::set_offset (float x, float y)
{
  m_x_offset = x;
  m_y_offset = y;
}

bool
ZoomGraphicContext::set_zoom (const float& z)
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
ZoomGraphicContext::zoom_to(float x1, float y1, float x2, float y2)
{
  float const center_x = (x1 + x2) / 2.0f;
  float const center_y = (y1 + y2) / 2.0f;

  float const width  = x2 - x1;
  float const height = y2 - y1;
  float const screen_relation = float(get_height())/float(get_width ());
  float const rect_relation   = height/width;

  //std::cout << "Screen: " << screen_relation << " Zoom: " << rect_relation << std::endl;
  if (rect_relation < screen_relation) // take width, ignore height
  {
    set_zoom(get_width()/width);
  }
  else // take height, ignore width
  {
    set_zoom(get_height()/height);
  }

  m_x_offset = (get_width()  / (2 * m_zoom)) - center_x;
  m_y_offset = (get_height() / (2 * m_zoom)) - center_y;
}

float
ZoomGraphicContext::get_width()
{
  return m_x2 - m_x1;
}

float
ZoomGraphicContext::get_height()
{
  return m_y2 - m_y1;
}

/* EOF */
