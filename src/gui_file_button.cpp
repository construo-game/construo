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

#include "gui_file_button.hpp"

GUIFileButton::GUIFileButton(const std::string& path) :
  GUIComponent(0, 0, 200, 150),
  m_path(path),
  m_mouse_over(false)
{
}

GUIFileButton::~GUIFileButton()
{
}

void
GUIFileButton::on_primary_button_press(float x, float y)
{
  on_click();
}

void
GUIFileButton::on_primary_button_release(float x, float y)
{
}

/* EOF */
