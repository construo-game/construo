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

GUIWindow::GUIWindow(std::string const& title) :
  WidgetGroup(),
  m_title(title),
  m_mouse_over(true)
{
  create<GUILabel>("Do you really want to quit?")->set_geometry(geom::frect(geom::fpoint(10.0f, 10.0f), geom::fsize(100.0f, 30.0f)));
  create<GUIButton>("Yes", yes_press)->set_geometry(geom::frect(geom::fpoint(10.0f, 50.0f), geom::fsize(80.0f, 25.0f)));
  create<GUIButton>("No", no_press)->set_geometry(geom::frect(geom::fpoint(110.0f, 50.0f), geom::fsize(80.0f, 25.0f)));
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

  WidgetGroup::draw(gc);
}

/* EOF */
