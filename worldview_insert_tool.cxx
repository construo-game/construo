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

#include "particle_factory.hxx"
#include "input_context.hxx"
#include "graphic_context.hxx"
#include "world.hxx"
#include "controller.hxx"
#include "worldview_component.hxx"
#include "worldview_insert_tool.hxx"

extern Controller* controller;

WorldViewInsertTool::WorldViewInsertTool (WorldViewComponent* c)
  : WorldViewTool(c) 
{
  current_particle = 0;
}

WorldViewInsertTool::~WorldViewInsertTool () 
{
}

void
WorldViewInsertTool::draw_background (GraphicContext* gc) 
{
  int x = worldview_component->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  int y = worldview_component->get_gc()->screen_to_world_y (input_context->get_mouse_y ());

  World& world = *controller->get_world();

  Particle* selected_particle = world.get_particle (x, y);
  if (selected_particle)
    {
      selected_particle->draw_highlight (gc);
    }
}

void
WorldViewInsertTool::draw_foreground (GraphicContext* gc) 
{
  World& world = *controller->get_world();

  int x = worldview_component->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  int y = worldview_component->get_gc()->screen_to_world_y (input_context->get_mouse_y ());

  Spring* selected_spring = world.get_spring (x, y);
  if (selected_spring)
    {
      selected_spring->draw_highlight (gc);
    }
      
  if (current_particle)
    {
      gc->draw_line (int(current_particle->pos.x), int(current_particle->pos.y),
                     x, y,
                     Color(0xAAAAAA));
    }
}

void
WorldViewInsertTool::on_primary_button_press (int screen_x, int screen_y) 
{
  World& world = *controller->get_world ();
  int x = worldview_component->get_gc()->screen_to_world_x (screen_x);
  int y = worldview_component->get_gc()->screen_to_world_y (screen_y);

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

void
WorldViewInsertTool::on_primary_button_release (int x, int y) 
{
}

void
WorldViewInsertTool::on_secondary_button_press (int screen_x, int screen_y)
{
  on_delete_press (screen_x, screen_y);
}

void
WorldViewInsertTool::on_secondary_button_release (int screen_x, int screen_y) 
{
}

void
WorldViewInsertTool::on_delete_press (int screen_x, int screen_y) 
{
  World& world = *controller->get_world ();

  int x = worldview_component->get_gc()->screen_to_world_x (screen_x);
  int y = worldview_component->get_gc()->screen_to_world_y (screen_y);

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

void
WorldViewInsertTool::on_fix_press (int screen_x, int screen_y) 
{
  int x = worldview_component->get_gc()->screen_to_world_x (screen_x);
  int y = worldview_component->get_gc()->screen_to_world_y (screen_y);

  Particle* particle = controller->get_world ()->get_particle (x, y);
  if (particle)
    {
      particle->set_fixed (!particle->get_fixed ());
    }
}

/* EOF */
