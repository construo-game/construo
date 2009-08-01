//  $Id$
//
//  Construo - A wire-frame construction gamee
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

#include "graphic_context.hpp"
#include "colors.hpp"
#include "gui_window.hpp"
#include "gui_label.hpp"
#include "gui_buttons.hpp"

void yes_press()
{
}

void no_press()
{
}

GUIWindow::GUIWindow (const std::string& t, int x, int y, int width, int height)
  : GUIChildManager (x, y, width, height),
    title (t)
{
  mouse_over = true;
  add (new GUILabel ("Do you really want to quit?", 10, 10, 100, 30));
  add (new GUIGenericButton("Yes", 10, 50, 80, 25, yes_press));
  add (new GUIGenericButton("No", 110, 50, 80, 25, no_press));
}

GUIWindow::~GUIWindow ()
{
}

void
GUIWindow::draw(GraphicContext* gc)
{
  std::cout << "Window draw.." << mouse_over << std::endl;
  /*if (mouse_over)
    {
      std::cout << "MOUSEOVER" << std::endl;
      gc->draw_fill_rect(x_pos, y_pos, x_pos + width, y_pos + height, Colors::button_bg_hover);
      gc->draw_rect(x_pos, y_pos, x_pos + width, y_pos + height, Colors::button_fg_hover);
    }
  else
    {
      std::cout << "MOUSEOVER NOT OVER" << std::endl;
      gc->draw_fill_rect(x_pos, y_pos, x_pos + width, y_pos + height, Colors::button_bg_passive);
      gc->draw_rect(x_pos, y_pos, x_pos + width, y_pos + height, Colors::button_fg_passive);
    }*/

  GUIChildManager::draw(gc);
}

/* EOF */
