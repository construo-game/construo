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

#ifndef HEADER_CONSTRUO_GUI_WINDOW_HPP
#define HEADER_CONSTRUO_GUI_WINDOW_HPP

#include <vector>
#include <string>
#include "zoom_graphic_context.hpp"
#include "widget_group.hpp"

namespace construo {

class GUIWindow : public WidgetGroup
{
public:
  GUIWindow(std::string const& title);
  ~GUIWindow();

  void draw(GraphicContext&) override;
  void on_mouse_enter() override { m_mouse_over = true; }
  void on_mouse_leave() override { m_mouse_over = false; }

private:
  std::string m_title;
  bool m_mouse_over;

public:
  GUIWindow(const GUIWindow&) = delete;
  GUIWindow& operator=(const GUIWindow&) = delete;
};

} // namespace construo

#endif

/* EOF */
