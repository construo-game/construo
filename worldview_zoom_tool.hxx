//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_WORLDVIEW_ZOOM_TOOL_HXX
#define HEADER_WORLDVIEW_ZOOM_TOOL_HXX

#include "vector2d.hxx"
#include "worldview_tool.hxx"

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
  WorldViewZoomTool (WorldViewComponent* c);
  ~WorldViewZoomTool ();
  
  void activate ();

  void draw_foreground (GraphicContext* gc);
  
  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);
};

#endif

/* EOF */
