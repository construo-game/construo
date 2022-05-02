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

#ifndef HEADER_WORLDVIEW_ZOOM_TOOL_HPP
#define HEADER_WORLDVIEW_ZOOM_TOOL_HPP

#include "vector2d.hpp"
#include "worldview_tool.hpp"

class WorldViewZoomTool : public WorldViewTool
{
public:
  WorldViewZoomTool();
  ~WorldViewZoomTool();

  void activate() override;

  void draw_foreground(ZoomGraphicContext& gc) override;
  void draw_background(ZoomGraphicContext& gc) override {}

  void on_primary_button_press(float x, float y) override;
  void on_primary_button_release(float x, float y) override;

  void on_secondary_button_press(float x, float y) override;
  void on_secondary_button_release(float x, float y) override;

private:
  /** Position of the mouse click, click_pos and the button_release
      pos form the rectangle. click_pos is in world coordinates */
  Vector2d m_click_pos;

  /** true if we are currently creating the zoom rectangle and
      click_pos is set, false otherwise */
  bool m_creating_zoom_rectangle;
};

#endif

/* EOF */
