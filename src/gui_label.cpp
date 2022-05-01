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

#include "graphic_context.hpp"
#include "colors.hpp"
#include "gui_label.hpp"

GUILabel::GUILabel (const std::string& title, int x, int y, int width, int height) :
  GUIComponent (x, y, width, height),
  m_title(title)
{
}

void
GUILabel::draw (GraphicContext* gc)
{
  gc->draw_string(m_x_pos + 10, m_y_pos + 15, m_title);
}

/* EOF */
