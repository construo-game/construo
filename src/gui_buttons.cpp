//  $Id$
//
//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include "graphic_context.hpp"
#include "controller.hpp"
#include "colors.hpp"
#include "world_gui_manager.hpp"
#include "gui_buttons.hpp"
#include "screen_manager.hpp"
#include "worldview_component.hpp"

#define BUTTON_POS(n) (80 + n * 30)
#define BUTTON_WIDTH  75
#define BUTTON_HEIGHT 25

GUIButton::GUIButton (const std::string& title_,
                      int x_pos_, int y_pos_, int width_, int height_)
  : GUIComponent (x_pos_, y_pos_, width_, height_),
    title (title_)
{
  mouse_over = false;
  pressed    = false;
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
GUIButton::on_primary_button_press (int x, int y)
{
  WorldGUIManager::instance()->grab_mouse (this);
  pressed = true;
}

void
GUIButton::on_primary_button_release (int x, int y)
{
  WorldGUIManager::instance()->ungrab_mouse (this);
  if (is_at (x, y))
    on_click ();
  pressed = false;
}

void
GUIButton::draw (GraphicContext* gc)
{
  if (pressed && mouse_over)
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

  if (pressed && mouse_over)
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
  gc->draw_string_centered (x_pos + width/2, y_pos + 16, title);
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
GUIButton::on_click()
{
  std::cout << "GUIButton: cliked (implement me)" << std::endl;
}

void
GUIRunButton::draw_content (GraphicContext* gc)
{
  if ((!pressed || !mouse_over) && Controller::instance()->is_running ())
    gc->draw_fill_rect (x_pos, y_pos,
                   x_pos + width, y_pos + height, Colors::button_bg_active) ;
  /*
  gc->draw_line (x_pos, y_pos,
                x_pos + width, y_pos + height,
                Color (0x0000FFFF));

  gc->draw_line (x_pos + width, y_pos,
                x_pos, y_pos + height,
                Color (0x0000FFFF));
  */
  GUIButton::draw_content (gc);
}

void
GUIRunButton::on_click()
{
  //std::cout << "Button pressed" << std::endl;
  Controller::instance()->start_simulation ();
}

GUISlowMoButton::GUISlowMoButton ()
  : GUIButton ("SlowMotion", 10, BUTTON_POS(1), BUTTON_WIDTH, BUTTON_HEIGHT)
{
  
}

void
GUISlowMoButton::on_click()
{
  Controller::instance()->set_slow_down (!Controller::instance()->slow_down_active ());
}

void
GUISlowMoButton::draw_content (GraphicContext* gc)
{
  if (Controller::instance()->slow_down_active())
    gc->draw_fill_rect (x_pos, y_pos,
                        x_pos + width, y_pos + height, Colors::button_bg_active);

  GUIButton::draw_content (gc);
}

GUIZoomInButton::GUIZoomInButton ()
  : GUIButton ("Zoom In", 10, BUTTON_POS(2), BUTTON_WIDTH, BUTTON_HEIGHT)
{  
}

void
GUIZoomInButton::on_click()
{
  WorldViewComponent::instance()->wheel_up (400,300);
}


GUIZoomOutButton::GUIZoomOutButton ()
  : GUIButton ("Zoom Out", 10, BUTTON_POS(3), BUTTON_WIDTH, BUTTON_HEIGHT)
{  
}

void
GUIZoomOutButton::on_click()
{
  WorldViewComponent::instance()->wheel_down (400,300); 
}

GUIQuitButton::GUIQuitButton ()
  : GUIButton ("Quit", 10, BUTTON_POS(12), BUTTON_WIDTH, BUTTON_HEIGHT)
{
}

void
GUIQuitButton::on_click()
{
  ScreenManager::instance()->quit();
}


GUILoadButton::GUILoadButton ()
  : GUIButton ("Load", 10, BUTTON_POS(9), BUTTON_WIDTH, BUTTON_HEIGHT)
{
}

void
GUILoadButton::on_click()
{
  ScreenManager::instance()->set_gui(ScreenManager::LOAD_GUI);
}

/* EOF */
