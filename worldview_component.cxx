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
#include "particle_factory.hxx"
#include "input_context.hxx"
#include "gui_manager.hxx"
#include "worldview_component.hxx"

extern GUIManager* gui_manager;
extern Controller* controller;

WorldViewComponent::WorldViewComponent ()
  : GUIComponent(0, 0, graphic_context->get_width (), graphic_context->get_height ())
{
  scrolling = true;
  current_particle = 0;
  //tool = INSERT_TOOL;
  tool = GROUP_MARK_TOOL;
  group_mark_mode = GROUP_MARK_NONE;
}

WorldViewComponent::~WorldViewComponent ()
{
  
}

void
WorldViewComponent::draw (GraphicContext* parent_gc)
{
  int x = gc.screen_to_world_x (input_context->get_mouse_x ());
  int y = gc.screen_to_world_y (input_context->get_mouse_y ());

  gc.set_parent_gc (parent_gc);

  gc.draw_line (-1000, 599, 1000, 599, Color (0x0000AA));

  World& world = *controller->get_world();

  if (tool == INSERT_TOOL)
    {
      Particle* selected_particle = world.get_particle (x, y);
      if (selected_particle)
        {
          selected_particle->draw_highlight (&gc);
        }
    }

  world.draw (&gc);
  if (tool == GROUP_MARK_TOOL)
    {
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->draw_highlight (&gc);
        }
    }

  if (tool == INSERT_TOOL)
    {
      Spring* selected_spring = world.get_spring (x, y);
      if (selected_spring)
        {
          selected_spring->draw_highlight (&gc);
        }
      
      if (current_particle)
        {
          gc.draw_line (int(current_particle->pos.x), int(current_particle->pos.y),
                        x, y,
                        Color(0xAAAAAA));
        }
    }
  else if (tool == GROUP_MARK_TOOL)
    {
      if (group_mark_mode == GETTING_SELECTION)
        {
          gc.draw_rect (int(selection_start.x),
                        int(selection_start.y),
                        x, y,
                        Color (0xDDDDDD));
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
WorldViewComponent::on_primary_button_press (int screen_x, int screen_y)
{
  World& world = *controller->get_world ();
  int x = gc.screen_to_world_x (screen_x);
  int y = gc.screen_to_world_y (screen_y);

  if (tool == INSERT_TOOL)
    {
      if (current_particle)
        {
          Particle* new_current_particle = world.get_particle (x, y);
          if (new_current_particle != current_particle)
            {
              if (new_current_particle) // connect to particles
                {
                  world.add_spring (current_particle, new_current_particle);
                }
              else // add a new particle and connect it with the current one
                {
                  new_current_particle = world.get_particle_mgr()->add_particle (Vector2d(x, y), Vector2d());
                  world.add_spring (current_particle, new_current_particle);
                }
              current_particle = 0;
            }
        }
      else
        {
          current_particle = world.get_particle (x, y);
          if (!current_particle)
            {
              Particle* p = world.get_particle_mgr()->add_particle (Vector2d(x, y), Vector2d());
              current_particle = p;
            }
        }
    }
  else if (tool == GROUP_MARK_TOOL)
    {
      gui_manager->grab_mouse (this);
      group_mark_mode = GETTING_SELECTION;
      Particle* new_current_particle = world.get_particle (x, y);
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          if (new_current_particle == *i)
            group_mark_mode = MOVING_SELECTION;
        }
      
      if (group_mark_mode == GETTING_SELECTION)
        {
          selection.clear ();
          selection_start.x = x;
          selection_start.y = y;
        }
    }
}

void
WorldViewComponent::on_primary_button_release (int screen_x, int screen_y)
{
  World& world = *controller->get_world ();
      
  if (tool == GROUP_MARK_TOOL)
    {
      gui_manager->ungrab_mouse (this);
      if (group_mark_mode == GETTING_SELECTION)
        { 
          selection = world.get_particles (int(selection_start.x), int(selection_start.y),
                                           gc.screen_to_world_x (screen_x),
                                           gc.screen_to_world_y (screen_y));
          group_mark_mode = GROUP_MARK_NONE;
        }
    }
}

void
WorldViewComponent::on_delete_press (int screen_x, int screen_y)
{
  if (tool == INSERT_TOOL)
    {
      World& world = *controller->get_world ();

      int x = gc.screen_to_world_x (screen_x);
      int y = gc.screen_to_world_y (screen_y);

      if (current_particle) 
        { // We are currently creating a new spring, abort that
          current_particle = 0;
        }
      else
        {
          Spring*   spring   = world.get_spring (x, y);
          Particle* particle = world.get_particle (x, y);

          if (particle)
            world.remove_particle (particle);
          else if (spring)
            world.remove_spring(spring);
        }
    }
}

void
WorldViewComponent::on_fix_press (int screen_x, int screen_y)
{
  if (tool == INSERT_TOOL)
    {
      int x = gc.screen_to_world_x (screen_x);
      int y = gc.screen_to_world_y (screen_y);

      Particle* particle = controller->get_world ()->get_particle (x, y);
      if (particle)
        {
          particle->set_fixed (!particle->get_fixed ());
        }
    }
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
WorldViewComponent::on_secondary_button_press (int x, int y)
{
  scrolling = true;
  x_offset = gc.get_x_offset ();
  y_offset = gc.get_y_offset (); 
  gui_manager->grab_mouse (this);

  scroll_pos_x = gc.screen_to_world_x(x);
  scroll_pos_y = gc.screen_to_world_y(y);
}

void
WorldViewComponent::on_secondary_button_release (int x, int y)
{
  scrolling = false;
  gui_manager->ungrab_mouse (this);
}

void
WorldViewComponent::on_mouse_move (int x, int y, int of_x, int of_y)
{
  World& world = *controller->get_world ();

  if (scrolling)
    {
      int new_scroll_pos_x = int(x/gc.get_zoom() - x_offset);
      int new_scroll_pos_y = int(y/gc.get_zoom() - y_offset);

      gc.set_offset (x_offset + (new_scroll_pos_x - scroll_pos_x),
                     y_offset + (new_scroll_pos_y - scroll_pos_y));

    }
  else if (tool == GROUP_MARK_TOOL && group_mark_mode == MOVING_SELECTION)
    {
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          // Will lead to round errors 
          (*i)->pos.x += of_x / gc.get_zoom();
          (*i)->pos.y += of_y / gc.get_zoom();

          std::vector<Spring*>& spring_mgr = world.get_spring_mgr();
          for (std::vector<Spring*>::iterator j = spring_mgr.begin (); 
               j != spring_mgr.end (); ++j)
            {
              if ((*j)->particles.first == *i
                  || (*j)->particles.second == *i)
                {
                  (*j)->recalc_length ();
                }
            }
        }
    }
}

/* EOF */
