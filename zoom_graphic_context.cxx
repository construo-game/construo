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

#include "zoom_graphic_context.hxx"

ZoomGraphicContext::ZoomGraphicContext ()
{
  x_offset   = 0;
  y_offset   = 0;
  zoom       = 1.0f;
  zoom_stage = 10;
  parent_gc  = NULL;
}

int
ZoomGraphicContext::screen_to_world_x (int x)
{
  return int((x / zoom) - x_offset);
}

int
ZoomGraphicContext::screen_to_world_y (int y)
{
  return int((y / zoom) - y_offset);
}

int
ZoomGraphicContext::world_to_screen_x (int x)
{
  return int((x + x_offset) * zoom);
}

int
ZoomGraphicContext::world_to_screen_y (int y) 
{
  return int((y + y_offset) * zoom);
}

void
ZoomGraphicContext::draw_line(int x1, int y1, int x2, int y2, Color color, int wide)
{
  parent_gc->draw_line(world_to_screen_x(x1),
                       world_to_screen_y(y1),
                       world_to_screen_x(x2),
                       world_to_screen_y(y2),
                       color, wide);
}

void
ZoomGraphicContext::draw_rect(int x1, int y1, int x2, int y2, Color color)
{
  parent_gc->draw_rect(world_to_screen_x(x1),
                       world_to_screen_y(y1),
                       world_to_screen_x(x2),
                       world_to_screen_y(y2),
                       color);
}

void
ZoomGraphicContext::draw_circle(int x, int y, int r, Color color)
{
  parent_gc->draw_circle(world_to_screen_x(x),
                         world_to_screen_y(y),
                         r,
                         color);
}

void
ZoomGraphicContext::draw_fill_circle(int x, int y, int r, Color color)
{
  parent_gc->draw_fill_circle(world_to_screen_x(x),
                              world_to_screen_y(y),
                              r,
                              color);
}
  
void
ZoomGraphicContext::draw_fill_rect(int x1, int y1, int x2, int y2, Color color)
{
  parent_gc->draw_fill_rect(world_to_screen_x(x1),
                            world_to_screen_y(y1),
                            world_to_screen_x(x2),
                            world_to_screen_y(y2),
                            color);
}

void
ZoomGraphicContext::draw_string(int x, int y, const std::string& str, Color color)
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
  int x = screen_to_world_x (screen_x);
  int y = screen_to_world_y (screen_y);

  x_offset = (x + x_offset)/1.2f - x;
  y_offset = (y + y_offset)/1.2f - y;
  
  zoom *= 1.2;

  return true;
}
bool
ZoomGraphicContext::zoom_out (int screen_x, int screen_y)
{
  int x = screen_to_world_x (screen_x);
  int y = screen_to_world_y (screen_y);

  x_offset = (x + x_offset)*1.2f - x;
  y_offset = (y + y_offset)*1.2f - y;
  
  zoom *= (1.0f/1.2f);

  return true;
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
  parent_gc->flip (world_to_screen_x (x1),
                   world_to_screen_y (y1),
                   world_to_screen_x (x2),
                   world_to_screen_y (y2));
}

/* EOF */
