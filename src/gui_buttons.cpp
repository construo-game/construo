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

GUIButton::GUIButton (const std::string& title,
                      float x, float y, float width, float height) :
  GUIComponent(x, y, width, height),
  m_title(title),
  m_mouse_over(false),
  m_pressed(false)
{
}

GUIButton::GUIButton (const std::string& title_) :
  GUIComponent(),
  m_title(title_),
  m_mouse_over(false),
  m_pressed(false)
{
}

void
GUIButton::on_mouse_enter ()
{
  m_mouse_over = true;
}

void
GUIButton::on_mouse_leave ()
{
  m_mouse_over = false;
}

void
GUIButton::on_primary_button_press (float x, float y)
{
  WorldGUIManager::instance()->grab_mouse (this);
  m_pressed = true;
}

void
GUIButton::on_primary_button_release (float x, float y)
{
  WorldGUIManager::instance()->ungrab_mouse (this);
  if (is_at (x, y))
    on_click ();
  m_pressed = false;
}

void
GUIButton::draw (GraphicContext& gc)
{
  if (m_pressed && m_mouse_over)
    {
      gc.draw_fill_rect (m_x, m_y, m_x + m_width,  m_y + m_height, Colors::button_bg_pressed);
    }
  else if (m_mouse_over)
    {
      gc.draw_fill_rect (m_x, m_y, m_x + m_width,  m_y + m_height, Colors::button_bg_hover);
    }
  else
    {
      gc.draw_fill_rect (m_x, m_y, m_x + m_width,  m_y + m_height, Colors::button_bg_passive);
    }

  draw_content (gc);

  if (m_pressed && m_mouse_over)
    {
      draw_border_pressed (gc);
    }
  else if (m_mouse_over)
    {
      draw_border_hover (gc);
    }
  else
    {
      draw_border_normal (gc);
    }
}

void
GUIButton::draw_content (GraphicContext& gc)
{
  gc.draw_string_centered (m_x + m_width/2, m_y + 16, m_title);
}

void
GUIButton::draw_border_hover(GraphicContext& gc)
{
  gc.draw_rect (m_x, m_y,
                 m_x + m_width, m_y + m_height, Colors::button_fg_hover);
}

void
GUIButton::draw_border_pressed(GraphicContext& gc)
{
  gc.draw_rect (m_x, m_y,
                 m_x + m_width, m_y + m_height, Colors::button_fg_pressed);
}

void
GUIButton::draw_border_normal(GraphicContext& gc)
{
  gc.draw_rect (m_x, m_y,
                 m_x + m_width, m_y + m_height, Colors::button_fg_passive);
}

/* EOF */
