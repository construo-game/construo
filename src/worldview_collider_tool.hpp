//  $Id$
// 
//  Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_WORLDVIEW_COLLIDER_TOOL_HPP
#define HEADER_CONSTRUO_WORLDVIEW_COLLIDER_TOOL_HPP

#include "vector2d.hpp"
#include "worldview_tool.hpp"

class Collider;

/** */
class WorldViewColliderTool : public WorldViewTool
{
private:
  bool creating_rect;
  Vector2d click_pos;

  Collider* to_delete_collider;
  Collider* move_collider;

  /** Get the collider on the given world cooridnates */
  Collider* get_collider (const Vector2d&);
public:
  WorldViewColliderTool ();
  ~WorldViewColliderTool ();

  void draw_background (ZoomGraphicContext* gc);
  void draw_foreground (ZoomGraphicContext* gc);

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);

  void on_mouse_move (int x, int y, int of_x, int of_y);
};

#endif

/* EOF */
