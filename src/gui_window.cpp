// Construo - A wire-frame construction gamee
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

#include "gui_window.hpp"

#include "graphic_context.hpp"
#include "colors.hpp"
#include "gui_label.hpp"
#include "gui_button.hpp"

void yes_press()
{
}

void no_press()
{
}

GUIWindow::GUIWindow (const std::string& t, float x, float y, float width, float height) :
  GUIChildManager(x, y, width, height),
  m_title(t),
  m_mouse_over(true)
{
  create<GUILabel>("Do you really want to quit?", 10, 10, 100, 30);
  create<GUIButton>("Yes", 10, 50, 80, 25, yes_press);
  create<GUIButton>("No", 110, 50, 80, 25, no_press);
}

GUIWindow::~GUIWindow ()
{
}

void
GUIWindow::draw(GraphicContext& gc)
{
  /*if (mouse_over)
    {
      gc.draw_fill_rect(x_pos, y_pos, x_pos + width, y_pos + height, g_style.button_bg_hover);
      gc.draw_rect(x_pos, y_pos, x_pos + width, y_pos + height, g_style.button_fg_hover);
    }
  else
    {
      gc.draw_fill_rect(x_pos, y_pos, x_pos + width, y_pos + height, g_style.button_bg_passive);
      gc.draw_rect(x_pos, y_pos, x_pos + width, y_pos + height, g_style.button_fg_passive);
    }*/

  GUIChildManager::draw(gc);
}

/* EOF */
