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
#include "colors.hxx"
#include "gui_manager.hxx"
#include "gui_buttons.hxx"
#include "worldview_component.hxx"

extern Controller* controller;
extern GUIManager* gui_manager;
extern WorldViewComponent* worldview_component;

#define BUTTON_POS(n) (50 + n * 30)
#define BUTTON_WIDTH  75
#define BUTTON_HEIGHT 25

GUIButton::GUIButton (const std::string& title_,
                      int x_pos_, int y_pos_, int width_, int height_)
  : title (title_), 
    x_pos (x_pos_), y_pos (y_pos_), width (width_), height (height_)
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
    {
      gc->draw_fill_rect (x_pos, y_pos, x_pos + width,  y_pos + height, Colors::button_bg_pressed);
    }
  else if (mouse_over)
    {
      gc->draw_fill_rect (x_pos, y_pos, x_pos + width,  y_pos + height, Colors::button_bg_hover);
    }
  else
    {
      gc->draw_fill_rect (x_pos, y_pos, x_pos + width,  y_pos + height, Colors::button_bg_passive);
    }

  draw_content (gc);

  if (pressed)
    {
      draw_border_pressed (gc);
    }
  else if (mouse_over)
    {
      draw_border_hover (gc);
    }
  else
    {
      draw_border_normal (gc);
    }
}

void
GUIButton::draw_content (GraphicContext* gc)
{
  gc->draw_string (x_pos + 10, y_pos + 16, title);
}

void
GUIButton::draw_border_hover(GraphicContext* gc)
{
  gc->draw_rect (x_pos, y_pos,
                 x_pos + width, y_pos + height, Colors::button_fg_hover);
}

void
GUIButton::draw_border_pressed(GraphicContext* gc)
{
  gc->draw_rect (x_pos, y_pos,
                 x_pos + width, y_pos + height, Colors::button_fg_pressed);
}

void
GUIButton::draw_border_normal(GraphicContext* gc)
{   
  gc->draw_rect (x_pos, y_pos,
                 x_pos + width, y_pos + height, Colors::button_fg_passive);
}

GUIRunButton::GUIRunButton ()
  : GUIButton ("Run", 10, BUTTON_POS(0), BUTTON_WIDTH, BUTTON_HEIGHT)
{
}

void
GUIRunButton::draw_content (GraphicContext* gc)
{
  if (controller->is_running ())
    gc->draw_fill_rect (x_pos, y_pos,
                   x_pos + width, y_pos + height, Colors::button_bg_active) ;
  /*
  gc->draw_line (x_pos, y_pos,
                x_pos + width, y_pos + height,
                Color (0x0000FF));

  gc->draw_line (x_pos + width, y_pos,
                x_pos, y_pos + height,
                Color (0x0000FF));
  */
  GUIButton::draw_content (gc);
}

void
GUIRunButton::on_primary_button_click (int x, int y)
{
  std::cout << "Button pressed" << std::endl;
  controller->start_simulation ();
}

GUISlowMoButton::GUISlowMoButton ()
  : GUIButton ("SlowMotion", 10, BUTTON_POS(1), BUTTON_WIDTH, BUTTON_HEIGHT)
{
  
}

void
GUISlowMoButton::on_primary_button_click (int x, int y)
{
  controller->set_slow_down (!controller->slow_down_active ());
}

void
GUISlowMoButton::draw_content (GraphicContext* gc)
{
  if (controller->slow_down_active())
    gc->draw_fill_rect (x_pos, y_pos,
                        x_pos + width, y_pos + height, Colors::button_bg_active);

  GUIButton::draw_content (gc);
}

GUIUndoButton::GUIUndoButton ()
  : GUIButton ("Undo", 10, BUTTON_POS(4), BUTTON_WIDTH, BUTTON_HEIGHT)
{  
}

void
GUIUndoButton::draw_content (GraphicContext* gc)
{
  GUIButton::draw_content (gc);
}

void
GUIUndoButton::on_primary_button_click (int x, int y)
{
  controller->undo ();
}

GUIZoomInButton::GUIZoomInButton ()
  : GUIButton ("Zoom In", 10, BUTTON_POS(2), BUTTON_WIDTH, BUTTON_HEIGHT)
{  
}

void
GUIZoomInButton::on_primary_button_click (int x, int y)
{
  worldview_component->wheel_up (400,300);
}


GUIZoomOutButton::GUIZoomOutButton ()
  : GUIButton ("Zoom Out", 10, BUTTON_POS(3), BUTTON_WIDTH, BUTTON_HEIGHT)
{  
}

void
GUIZoomOutButton::on_primary_button_click (int x, int y)
{
  worldview_component->wheel_down (400,300); 
}

GUIQuitButton::GUIQuitButton ()
  : GUIButton ("Quit", 10, BUTTON_POS(6), BUTTON_WIDTH, BUTTON_HEIGHT)
{
}

void
GUIQuitButton::on_primary_button_click (int x, int y)
{
  gui_manager->quit();
}

/* EOF */
