//  $Id$
//
//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <iostream>
#include "math.hxx"
#include "zoom_graphic_context.hxx"

ZoomGraphicContext::ZoomGraphicContext ()
{
  x_offset   = 0;
  y_offset   = 0;
  zoom       = 1.0f;
  parent_gc  = NULL;
}

float
ZoomGraphicContext::screen_to_world_x (float x)
{
  return (x / zoom) - x_offset;
}

float
ZoomGraphicContext::screen_to_world_y (float y)
{
  return (y / zoom) - y_offset;
}

float
ZoomGraphicContext::world_to_screen_x (float x)
{
  return (x + x_offset) * zoom;
}

float
ZoomGraphicContext::world_to_screen_y (float y) 
{
  return (y + y_offset) * zoom;
}

void
ZoomGraphicContext::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  parent_gc->draw_line(world_to_screen_x(x1),
                       world_to_screen_y(y1),
                       world_to_screen_x(x2),
                       world_to_screen_y(y2),
                       color, wide);
}

void
ZoomGraphicContext::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  parent_gc->draw_rect(world_to_screen_x(x1),
                       world_to_screen_y(y1),
                       world_to_screen_x(x2),
                       world_to_screen_y(y2),
                       color);
}

void
ZoomGraphicContext::draw_circle(float x, float y, float r, Color color)
{
  parent_gc->draw_circle(world_to_screen_x(x),
                         world_to_screen_y(y),
                         Math::max(2.0f, r * zoom),
                         color);
}

void
ZoomGraphicContext::draw_fill_circle(float x, float y, float r, Color color)
{
  parent_gc->draw_fill_circle(world_to_screen_x(x),
                              world_to_screen_y(y),
                              Math::max(2.0f, r * zoom),
                              color);
}
  
void
ZoomGraphicContext::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  parent_gc->draw_fill_rect(world_to_screen_x(x1),
                            world_to_screen_y(y1),
                            world_to_screen_x(x2),
                            world_to_screen_y(y2),
                            color);
}

void
ZoomGraphicContext::draw_string(float x, float y, const std::string& str, Color color)
{
  parent_gc->draw_string(world_to_screen_x(x),
                         world_to_screen_y(y),
                         str,
                         color);
}

void
ZoomGraphicContext::set_parent_gc (GraphicContext* gc)
{
  parent_gc = gc;
}

bool
ZoomGraphicContext::zoom_in (int screen_x, int screen_y)
{
  float x = screen_to_world_x (screen_x);
  float y = screen_to_world_y (screen_y);

  //std::cout << "Zoom: " << screen_x << " " << screen_y
  //<< " " << x << " " << y << std::endl;

  //set_zoom(zoom * 1.2);
  //move_to (x, y);

  x_offset = (x + x_offset)/1.2f - x;
  y_offset = (y + y_offset)/1.2f - y;
  
  zoom *= 1.2;

  return true;
}
bool
ZoomGraphicContext::zoom_out (int screen_x, int screen_y)
{
  float x = screen_to_world_x (screen_x);
  float y = screen_to_world_y (screen_y);

  x_offset = (x + x_offset)*1.2f - x;
  y_offset = (y + y_offset)*1.2f - y;

  //set_zoom(zoom / 1.2f);
  //move_to (x, y);
  zoom *= (1.0f/1.2f);


  return true;
}

void
ZoomGraphicContext::move_to (float x, float y)
{
  x_offset = (get_width()  / (2*zoom)) + x;
  y_offset = (get_height() / (2*zoom)) + y;
}

void
ZoomGraphicContext::translate_offset (int x, int y)
{
  x_offset -= x;
  y_offset -= y;
}

void
ZoomGraphicContext::set_offset (float x, float y)
{
  x_offset = x;
  y_offset = y;
}

void
ZoomGraphicContext::flip (int x1, int y1, int x2, int y2)
{
  assert (false);
  /*  parent_gc->flip (world_to_screen_x (x1),
                   world_to_screen_y (y1),
                   world_to_screen_x (x2),
                   world_to_screen_y (y2));*/
}

bool
ZoomGraphicContext::set_zoom (const float& z)
{
  const float max_zoom = 20.0f;
  const float min_zoom = 0.05f;
  
  if (z > max_zoom)
    {
      zoom = max_zoom;
      return false;
    }
  else if (z < min_zoom)
    {
      zoom = min_zoom;
      return false;
    }
  else
    {
      zoom = z;
      return true;
    }
}

void
ZoomGraphicContext::zoom_to (int x1, int y1, int x2, int y2)
{
  std::cout << "Zooming to: " << x1 << " " << y1 << " " << x2 << " " << y2
            << std::endl;
  
  float center_x = (x1 + x2) / 2.0f;
  float center_y = (y1 + y2) / 2.0f;

  float width  = x2 - x1;
  float height = y2 - y1;
  float screen_relation = get_width ()/get_height();
  float rect_relation   = width/height; 
  
  
  if (rect_relation > screen_relation)
    {
      set_zoom(800/width);
    }
  else
    {
      set_zoom(600/height);
    }

  x_offset = (get_width()  / (2*zoom)) - center_x;
  y_offset = (get_height() / (2*zoom)) - center_y;
}

/* EOF */
