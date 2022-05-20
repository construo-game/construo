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

#include "gui_file_manager.hpp"
#include "gui_directory_button.hpp"

namespace construo {

GUIDirectoryButton::GUIDirectoryButton(const std::string& pathname,
                                       std::function<void ()> callback) :
  GUIFileButton(pathname),
  m_callback(callback)
{
}

GUIDirectoryButton::~GUIDirectoryButton ()
{
}

void
GUIDirectoryButton::draw (GraphicContext& parent_gc)
{
  parent_gc.draw_fill_rect(m_geometry, Color (0xBB0000FF));

  parent_gc.draw_string(m_geometry.topleft() + geom::foffset(40.0f, 20.0f), "..:: Directory ::..");
  parent_gc.draw_string(m_geometry.topleft() + geom::foffset(30.0f, 40.0f), m_path);

  if (m_mouse_over) {
    parent_gc.draw_rect (m_geometry, Color (0xFFFFFFFF));
  } else {
    parent_gc.draw_rect (m_geometry, Color (0xFF0000FF));
  }
}

void
GUIDirectoryButton::on_click()
{
  m_callback();
}

} // namespace construo

/* EOF */
