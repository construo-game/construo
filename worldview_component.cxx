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

#include "colors.hxx"
#include "controller.hxx"
#include "world_gui_manager.hxx"
#include "worldview_tool.hxx"
#include "worldview_insert_tool.hxx"
#include "worldview_select_tool.hxx"
#include "worldview_zoom_tool.hxx"
#include "worldview_collider_tool.hxx"
#include "worldview_component.hxx"

WorldViewComponent* WorldViewComponent::instance_;

WorldViewComponent::WorldViewComponent ()
  : GUIComponent(0, 0, graphic_context->get_width (), graphic_context->get_height ())
{
  instance_ = this;

  scrolling = false;
  use_grid  = false;

  select_tool   = new WorldViewSelectTool ();
  insert_tool   = new WorldViewInsertTool ();
  zoom_tool     = new WorldViewZoomTool ();
  collider_tool = new WorldViewColliderTool ();

  current_tool = insert_tool;
  mode = INSERT_MODE;
}

void
WorldViewComponent::set_mode (Mode m)
{
  current_tool->deactivate ();

  if (m == INSERT_MODE)
    {
      current_tool = insert_tool;
      mode = INSERT_MODE;
    }
  else if (m == SELECT_MODE)
    {
      current_tool = select_tool;
      mode = SELECT_MODE;
    }
  else if (m == ZOOM_MODE)
    {
      current_tool = zoom_tool;
      mode = ZOOM_MODE;
    }
  else if (m == COLLIDER_MODE)
    {
      current_tool = collider_tool;
      mode = COLLIDER_MODE;
    }
  else
    {
      std::cout << "Unknown Mode" << std::endl;
      assert (false);
    }

  current_tool->activate ();
}

WorldViewComponent::~WorldViewComponent ()
{
  instance_ = 0;
}

void
WorldViewComponent::draw_grid()
{
  if (gc.get_zoom() > 0.4f)
    {
      Color color = Colors::grid_color;
      int start_x = Math::round_to(gc.screen_to_world_x(0), 10) - 10;
      int end_x   = Math::round_to(gc.screen_to_world_x(gc.get_width()), 10) + 10;

      int start_y = Math::round_to(gc.screen_to_world_y(0), 10) - 10;
      int end_y   = Math::round_to(gc.screen_to_world_y(gc.get_height()), 10) + 10;

      gc.push_quick_draw();
      for(int y = start_y; y < end_y; y += 10)
        gc.draw_line(start_x, y, 
                     end_x, y,
                     color, 1);

      for(int x = start_x; x < end_x; x += 10)
        gc.draw_line(x, start_y, 
                     x, end_y,
                     color, 1);
      gc.pop_quick_draw();
    }
}

void
WorldViewComponent::draw (GraphicContext* parent_gc)
{
  //int x = gc.screen_to_world_x (input_context->get_mouse_x ());
  //int y = gc.screen_to_world_y (input_context->get_mouse_y ());

  gc.set_parent_gc (parent_gc);

  if (use_grid)
    draw_grid();

  // Draw the buttom border line
  if (gc.screen_to_world_y(gc.get_height()) >= 599)
    gc.draw_fill_rect(gc.screen_to_world_x(0), 
                      599, // FIXME: 599 is a relict from earlier versions, should be 0
                      gc.screen_to_world_x(gc.get_width()),
                      gc.screen_to_world_y(gc.get_height()), // FIXME: 599 is a relict from earlier versions, should be 0
                      Colors::ground_color);

  World& world = *Controller::instance()->get_world();

  if (Controller::instance()->get_action_cam()
      && Controller::instance()->is_running())
    {
      // Live Action Cam
      const WorldBoundingBox& box = world.calc_bounding_box();
      gc.zoom_to((int) box.x1, (int)box.y1,
                 (int)box.x2, (int)box.y2);
      gc.zoom_out (get_width()/2, get_height()/2);
      gc.zoom_out (get_width()/2, get_height()/2);
    }

  current_tool->draw_background (&gc);

  world.draw_colliders (&gc);
  world.draw_springs (&gc);
  if (!Controller::instance()->get_hide_dots())
    world.draw_particles (&gc);

  current_tool->draw_foreground (&gc);

  if (0)
    {
      switch (mode)
        {
        case ZOOM_MODE:
          parent_gc->draw_string (10, parent_gc->get_height () - 15, "[  Zoom Mode  ]");
          break;
        case INSERT_MODE:
          parent_gc->draw_string (10, parent_gc->get_height () - 15, "[ Insert Mode ]");
          break;
        case SELECT_MODE:
          parent_gc->draw_string (10, parent_gc->get_height () - 15, "[ Select Mode ]");
          break;
        case COLLIDER_MODE:
          parent_gc->draw_string (10, parent_gc->get_height () - 15, "[Collider Mode]");
          break;
        }
    }

  //const WorldBoundingBox& box = world.calc_bounding_box();
  //gc.flip (int(box.x1), int(box.y1), int(box.x2), int(box.y2));
}

void
WorldViewComponent::wheel_up (int x, int y)
{
  gc.zoom_in (x, y);
}

void
WorldViewComponent::wheel_down (int x, int y)
{
  gc.zoom_out (x, y);
}

void
WorldViewComponent::on_button_press (int button_id, int x, int y)
{
  current_tool->on_button_press (button_id, x, y);
}

void
WorldViewComponent::on_primary_button_press (int screen_x, int screen_y)
{
  current_tool->on_primary_button_press (screen_x, screen_y);
}

void
WorldViewComponent::on_primary_button_release (int screen_x, int screen_y)
{
  current_tool->on_primary_button_release (screen_x, screen_y);
}

void
WorldViewComponent::on_secondary_button_press (int screen_x, int screen_y)
{
  current_tool->on_secondary_button_press (screen_x, screen_y);
}

void
WorldViewComponent::on_secondary_button_release (int screen_x, int screen_y)
{
  current_tool->on_secondary_button_release (screen_x, screen_y);
}

void
WorldViewComponent::on_delete_press (int screen_x, int screen_y)
{
  current_tool->on_delete_press (screen_x, screen_y);
}

void
WorldViewComponent::on_duplicate_press (int screen_x, int screen_y)
{
  current_tool->on_duplicate_press (screen_x, screen_y);
}

void
WorldViewComponent::on_fix_press (int screen_x, int screen_y)
{
  current_tool->on_fix_press (screen_x, screen_y);
}

void
WorldViewComponent::scroll_left ()
{
  gc.translate_offset (-20, 0);
}

void
WorldViewComponent::scroll_right ()
{
  gc.translate_offset (20, 0);
}

void
WorldViewComponent::scroll_up ()
{
  gc.translate_offset (0, -20);
}

void
WorldViewComponent::scroll_down ()
{
  gc.translate_offset (0, 20);
}

void
WorldViewComponent::on_tertiary_button_press (int x, int y)
{
  scrolling = true;
  x_offset = gc.get_x_offset ();
  y_offset = gc.get_y_offset (); 
  WorldGUIManager::instance()->grab_mouse (this);

  scroll_pos_x = gc.screen_to_world_x(x);
  scroll_pos_y = gc.screen_to_world_y(y);
}

void
WorldViewComponent::on_tertiary_button_release (int x, int y)
{
  scrolling = false;
  WorldGUIManager::instance()->ungrab_mouse (this);
}

void
WorldViewComponent::on_mouse_move (int x, int y, int of_x, int of_y)
{
  if (scrolling)
    {
      int new_scroll_pos_x = int(x/gc.get_zoom() - x_offset);
      int new_scroll_pos_y = int(y/gc.get_zoom() - y_offset);

      gc.set_offset (x_offset + (new_scroll_pos_x - scroll_pos_x),
                     y_offset + (new_scroll_pos_y - scroll_pos_y));

    }
  else
    {
      current_tool->on_mouse_move (x, y, of_x, of_y);
    }
}

void
WorldViewComponent::on_grid_press(int x, int y)
{
  use_grid = !use_grid;
}

float
WorldViewComponent::get_zoom ()
{
  return gc.get_zoom ();
}

/* EOF */
