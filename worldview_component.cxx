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
{
  scrolling = true;
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

  Particle* selected_particle = world.get_particle (x, y);
  if (selected_particle)
    {
      selected_particle->draw_highlight (&gc);
    }

  world.draw (&gc);

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
WorldViewComponent::on_primary_button_click (int screen_x, int screen_y)
{
  int x = gc.screen_to_world_x (screen_x);
  int y = gc.screen_to_world_y (screen_y);

  World& world = *controller->get_world ();

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
              new_current_particle = world.get_particle_mgr()->add_particle (CL_Vector(x, y), CL_Vector());
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
          Particle* p = world.get_particle_mgr()->add_particle (CL_Vector(x, y), CL_Vector());
          current_particle = p;
        }
    }
}

void
WorldViewComponent::on_delete_press (int screen_x, int screen_y)
{
  int x = gc.screen_to_world_x (screen_x);
  int y = gc.screen_to_world_y (screen_y);

  std::cout << "Deleteing " << current_particle << std::endl;
  if (current_particle)
    {
      current_particle = 0;
    }
  else
    {
      Particle* p = controller->get_world ()->get_particle (x, y);
      controller->get_world ()->remove_particle (p);
    }
}

void
WorldViewComponent::on_fix_press (int screen_x, int screen_y)
{
  int x = gc.screen_to_world_x (screen_x);
  int y = gc.screen_to_world_y (screen_y);

  Particle* particle = controller->get_world ()->get_particle (x, y);
  std::cout << "Fixing particle: " << particle << std::endl;
  if (particle)
    {
      std::cout << "particle: " << particle->get_fixed () << std::endl;
      particle->set_fixed (!particle->get_fixed ());
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
  std::cout << "Secondary PRESS: " << std::endl;
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
  std::cout << "Secondary RELEASE: " << std::endl;
  scrolling = false;
  gui_manager->ungrab_mouse (this);
}

void
WorldViewComponent::on_mouse_move (int x, int y, int of_x, int of_y)
{
  std::cout << "Move: " << x << " " << y << std::endl;

  if (scrolling)
    {
      int new_scroll_pos_x = int(x/gc.get_zoom() - x_offset);
      int new_scroll_pos_y = int(y/gc.get_zoom() - y_offset);

      gc.set_offset (x_offset + (new_scroll_pos_x - scroll_pos_x),
                     y_offset + (new_scroll_pos_y - scroll_pos_y));

      std::cout << "X_offset: " << x << " " << y << std::endl;
    }
}

/* EOF */
