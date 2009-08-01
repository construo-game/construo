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

#ifndef HEADER_WORLDVIEW_ZOOM_TOOL_HPP
#define HEADER_WORLDVIEW_ZOOM_TOOL_HPP

#include "vector2d.hpp"
#include "worldview_tool.hpp"

/** */
class WorldViewZoomTool : public WorldViewTool
{
private:
  /** Position of the mouse click, click_pos and the button_release
      pos form the rectangle. click_pos is in world coordinates */
  Vector2d click_pos;

  /** true if we are currently creating the zoom rectangle and
      click_pos is set, false otherwise */
  bool creating_zoom_rectangle;
public:
  WorldViewZoomTool ();
  ~WorldViewZoomTool ();
  
  void activate ();

  void draw_foreground (ZoomGraphicContext* gc);
  void draw_background (ZoomGraphicContext* gc) {}
  
  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);
};

#endif

/* EOF */
