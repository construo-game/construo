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

#ifndef HEADER_CONSTRUO_GUI_FILE_BUTTON_HPP
#define HEADER_CONSTRUO_GUI_FILE_BUTTON_HPP

#include <string>
#include "zoom_graphic_context.hpp"
#include "gui_component.hpp"

class GUIFileButton : public GUIComponent
{
public:
  GUIFileButton(std::string const& path);
  ~GUIFileButton();

  virtual void on_click() =0;

  void on_mouse_enter() override { m_mouse_over = true; }
  void on_mouse_leave() override { m_mouse_over = false; }

  void on_primary_button_press(geom::fpoint const& pos) override;
  void on_primary_button_release(geom::fpoint const& pos) override;

protected:
  std::string m_path;
  bool m_mouse_over;
};

#endif

/* EOF */
