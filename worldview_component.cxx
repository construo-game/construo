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

#include "controller.hxx"
#include "gui_manager.hxx"
#include "worldview_tool.hxx"
#include "worldview_insert_tool.hxx"
#include "worldview_select_tool.hxx"
#include "worldview_zoom_tool.hxx"
#include "worldview_component.hxx"

WorldViewComponent* WorldViewComponent::instance_;

WorldViewComponent::WorldViewComponent ()
  : GUIComponent(0, 0, graphic_context->get_width (), graphic_context->get_height ())
{
  instance_ = this;

  scrolling = false;

  select_tool  = new WorldViewSelectTool ();
  insert_tool  = new WorldViewInsertTool ();
  zoom_tool    = new WorldViewZoomTool ();

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
  else
    {
      std::cout << "Unknown Mode" << std::endl;
      assert (false);
    }

  current_tool->activate ();

  std::cout << "Setting Mode: " << m << " " << current_tool << std::endl;
}

WorldViewComponent::~WorldViewComponent ()
{
  instance_ = 0;
}

void
WorldViewComponent::draw (GraphicContext* parent_gc)
{
  //int x = gc.screen_to_world_x (input_context->get_mouse_x ());
  //int y = gc.screen_to_world_y (input_context->get_mouse_y ());

  gc.set_parent_gc (parent_gc);

  gc.draw_line (-1000, 599, 1000, 599, Color (0x0000AA));

  World& world = *Controller::instance()->get_world();

  if (0)
    if (Controller::instance()->is_running())
      {
        // Live Action Cam
        const WorldBoundingBox& box = world.calc_bounding_box();
        gc.zoom_to((int) box.x1, (int)box.y1,
                   (int)box.x2, (int)box.y2);
      }

  current_tool->draw_background (&gc);
  world.draw (&gc);
  current_tool->draw_foreground (&gc);

  switch (mode)
    {
    case ZOOM_MODE:
      parent_gc->draw_string (10, parent_gc->get_height () - 15, "[ Zoom Mode ]");
      break;
    case INSERT_MODE:
      parent_gc->draw_string (10, parent_gc->get_height () - 15, "[Insert Mode]");
      break;
    case SELECT_MODE:
      parent_gc->draw_string (10, parent_gc->get_height () - 15, "[Select Mode]");
      break;
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
  GUIManager::instance()->grab_mouse (this);

  scroll_pos_x = gc.screen_to_world_x(x);
  scroll_pos_y = gc.screen_to_world_y(y);
}

void
WorldViewComponent::on_tertiary_button_release (int x, int y)
{
  scrolling = false;
  GUIManager::instance()->ungrab_mouse (this);
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

float
WorldViewComponent::get_zoom ()
{
  return gc.get_zoom ();
}

/* EOF */
