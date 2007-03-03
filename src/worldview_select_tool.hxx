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

#ifndef HEADER_CONSTRUO_WORLDVIEW_SELECT_TOOL_HXX
#define HEADER_CONSTRUO_WORLDVIEW_SELECT_TOOL_HXX

#include "selection.hxx"
#include "worldview_tool.hxx"

class Particle;

/** */
class WorldViewSelectTool : public WorldViewTool
{
private:
  Selection selection;

  typedef enum { GETTING_SELECTION_MODE, 
                 MOVING_SELECTION_MODE, 
                 ROTATING_SELECTION_MODE,
                 SCALING_SELECTION_MODE,
                 IDLE_MODE } Mode;
  Mode mode;

  /** The start position of a click & drap operation (aka move or
      rotate), in world coordinates */
  Vector2d click_pos;
  
  /** The difference the selection was moved on the last mouse move */
  Vector2d move_diff;

  /** The particle, by which is selection grabbed */
  Particle* move_current_particle;

  /** The center of a rotation */
  Vector2d rotate_center;

  /** The center of a scalation */
  Vector2d scale_center;
  float old_scale_factor;
public:
  WorldViewSelectTool ();
  ~WorldViewSelectTool ();

  void activate ();
  void deactivate ();

  void draw_background (ZoomGraphicContext* gc);
  void draw_foreground (ZoomGraphicContext* gc);

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);

  void on_mouse_move (int x, int y, int of_x, int of_y);

  void on_button_press (int button_id, int x, int y);

  void on_scale_press (int x, int y);
  void on_flip_press (int x, int y);
  void on_duplicate_press (int x, int y);
  void on_delete_press (int x, int y);
  void on_fix_press (int x, int y);
  void on_join_press (int x, int y);
};

#endif

/* EOF */
