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
#include "graphic_context.hxx"
#include "controller.hxx"
#include "gui_buttons.hxx"

extern Controller* controller;

GUIButton::GUIButton (int x_pos_, int y_pos_, int width_, int height_)
  : x_pos (x_pos_), y_pos (y_pos_), width (width_), height (height_)
{
  mouse_over = false;
  pressed    = false;
}

bool
GUIButton::is_at (int x, int y)
{
  return (x    >= x_pos
          && y >= y_pos
          && x < x_pos + width
          && y < y_pos + height);
}

void
GUIButton::on_mouse_enter ()
{
  mouse_over = true;
}

void
GUIButton::on_mouse_leave ()
{
  mouse_over = false;
}

void
GUIButton::draw (GraphicContext* gc)
{
  if (pressed)
    draw_border_pressed (gc);
  else if (mouse_over)
    draw_border_hover (gc);
  else
    draw_border_normal (gc);

  draw_content (gc);
}


void
GUIButton::draw_border_hover(GraphicContext* gc)
{
  gc->draw_rect (x_pos, y_pos,
                 x_pos + 50, y_pos + 50, Color(0xFFFFFF));
}

void
GUIButton::draw_border_pressed(GraphicContext* gc)
{
  gc->draw_rect (x_pos, y_pos,
                 x_pos + 50, y_pos + 50, Color(0x999999));
}

void
GUIButton::draw_border_normal(GraphicContext* gc)
{
  if (controller->is_running ())
    gc->draw_fill_rect (x_pos, y_pos,
                   x_pos + 50, y_pos + 50, Color(0xFF0000));
    
  gc->draw_rect (x_pos, y_pos,
                 x_pos + 50, y_pos + 50, Color(0xFF0000));
}

GUIRunButton::GUIRunButton ()
  : GUIButton (10, 100, 50, 50)
{
}

void
GUIRunButton::draw_content (GraphicContext* gc)
{
  if (controller->is_running ())
    gc->draw_fill_rect (x_pos, y_pos,
                   x_pos + 50, y_pos + 50, Color(0xFF0000));

  gc->draw_line (x_pos, y_pos,
                x_pos + width, y_pos + height,
                Color (0x0000FF));

  gc->draw_line (x_pos + width, y_pos,
                x_pos, y_pos + height,
                Color (0x0000FF));
}

void
GUIRunButton::on_primary_button_click (int x, int y)
{
  std::cout << "Button pressed" << std::endl;
  controller->start_simulation ();
}


GUIUndoButton::GUIUndoButton ()
  : GUIButton (10, 160, 50, 50)
{  
}

void
GUIUndoButton::draw_content (GraphicContext*)
{
  
}

void
GUIUndoButton::on_primary_button_click (int x, int y)
{
  controller->undo ();
}

/* EOF */
