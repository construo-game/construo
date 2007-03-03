//  $Id$
// 
//  Construo - A wire-frame construction game
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

#ifndef HEADER_WORLDVIEW_COMPONENT_HXX
#define HEADER_WORLDVIEW_COMPONENT_HXX

#include "input_context.hxx"
#include "zoom_graphic_context.hxx"
#include "gui_component.hxx"

class Particle;
class WorldViewTool;
class WorldViewInsertTool;
class WorldViewSelectTool;
class WorldViewZoomTool;
class WorldViewColliderTool;

/** GUI Component that manages the view and the editing of the
    world, this is the main component of Construo */
class WorldViewComponent : public GUIComponent
{
public:
  enum Mode { INSERT_MODE, SELECT_MODE, ZOOM_MODE, COLLIDER_MODE };

private:
  Mode mode;

  ZoomGraphicContext gc;

  /** If set to true, display a grid and align dots to it */
  bool use_grid;
  float grid_base_size;
  int grid_constant;
  float grid_scale_factor;
  float grid_snap_factor;
  
  /** True if the third button is currently pressed and we are in
      scroll mode */
  bool scrolling;
  
  float scroll_pos_x;
  float scroll_pos_y;

  float x_offset;
  float y_offset;

  /** If the user clicks in this component, the tool decides what to
      do */
  WorldViewTool*       current_tool;
  WorldViewSelectTool* select_tool;
  WorldViewInsertTool* insert_tool;
  WorldViewZoomTool*   zoom_tool;
  WorldViewColliderTool*   collider_tool;

  static WorldViewComponent* instance_;

  /** Draw the background grid to which new dots align */
  void draw_grid();
  
  /** Draw ground, aka the all overspanning collider at the bottem */
  void draw_ground();
public:
  void set_mode (Mode m); 
  Mode get_mode () { return mode; }

  WorldViewComponent ();
  ~WorldViewComponent ();

  static inline WorldViewComponent* instance() { return instance_; }

  WorldViewInsertTool* get_insert_tool() { return insert_tool; }

  ZoomGraphicContext* get_gc () { return &gc; }
  void draw (GraphicContext* parent_gc);

  bool uses_grid() const { return  use_grid; }
  float get_grid_size();
  float get_snap_size();

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);

  void on_tertiary_button_press (int x, int y);
  void on_tertiary_button_release (int x, int y);

  void on_mouse_move (int x, int y, int of_x, int of_y);

  void on_scale_press(int x, int y);
  void on_grid_press(int x, int y);
  void on_duplicate_press (int x, int y);
  void on_delete_press (int x, int y);
  void on_fix_press (int x, int y);
  void on_join_press (int x, int y);

  void on_button_press (int button_id, int x, int y);

  void wheel_up (int x, int y);
  void wheel_down (int x, int y);

  void scroll_left ();
  void scroll_right ();
  void scroll_up ();
  void scroll_down ();

  float get_zoom (); 

  /** Called once the world has changed, aka a new world got loaded */
  void on_world_change();

  /** */
  bool is_at (int x, int y) { return true; }
};

#endif

/* EOF */
