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

#include "colors.hxx"
#include "particle_factory.hxx"
#include "input_context.hxx"
#include "graphic_context.hxx"
#include "world.hxx"
#include "controller.hxx"
#include "worldview_component.hxx"
#include "worldview_insert_tool.hxx"
#include "world_gui_manager.hxx"

WorldViewInsertTool::WorldViewInsertTool ()
{
  current_particle = 0;
  particle_mass = 0.1f;
}

WorldViewInsertTool::~WorldViewInsertTool () 
{
}

void
WorldViewInsertTool::draw_background (ZoomGraphicContext* gc) 
{
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (input_context->get_mouse_y ());

  World& world = *Controller::instance()->get_world();

  Particle* selected_particle = world.get_particle (x, y);
  if (selected_particle)
    {
      selected_particle->draw_highlight(gc);
    }
}

void
WorldViewInsertTool::draw_foreground (ZoomGraphicContext* gc) 
{
  World& world = *Controller::instance()->get_world();

  Vector2d click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world (input_context->get_mouse_pos ());

  Vector2d new_particle_pos;
          
  if (WorldViewComponent::instance()->uses_grid())
    new_particle_pos = Vector2d(Math::round_to(click_pos.x, 10),
                                Math::round_to(click_pos.y, 10));
  else
    new_particle_pos = Vector2d(click_pos.x, click_pos.y);
  
  Spring* selected_spring = world.get_spring (click_pos.x, click_pos.y);

  if (selected_spring)
    {
      selected_spring->draw_highlight (gc);
    }
      
  if (current_particle)
    {
      gc->GraphicContext::draw_line (current_particle->pos, new_particle_pos,
                                     Colors::new_spring, 2);
    }

  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (input_context->get_mouse_y ());

  Particle* selected_particle = world.get_particle (x, y);
  if (selected_particle)
    {
      selected_particle->draw_infos (gc);
    }
  else
    {
      gc->draw_fill_circle(new_particle_pos.x,
                           new_particle_pos.y,
                           3.0f,
                           Colors::highlight);
    }
}

void
WorldViewInsertTool::on_primary_button_press (int screen_x, int screen_y) 
{
  World& world = *Controller::instance()->get_world ();
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x);
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y);

  if (current_particle)
    {
      // We are going to place a second particle and convert the last an
      // the new one with a spring
      Particle* new_current_particle = world.get_particle (x, y);
      if (new_current_particle != current_particle)
        {
          if (new_current_particle) // connect to particles
            {
              world.add_spring (current_particle, new_current_particle);
            }
          else // add a new particle and connect it with the current one
            {
              Vector2d new_particle_pos;
              if (WorldViewComponent::instance()->uses_grid())
                new_particle_pos = Vector2d(Math::round_to(x, 10),
                                            Math::round_to(y, 10));
              else
                new_particle_pos = Vector2d(x, y);

              new_current_particle = world.get_particle_mgr()->add_particle(new_particle_pos,
                                                                            Vector2d(), 
                                                                            particle_mass);
              world.add_spring (current_particle, new_current_particle);
            }
          current_particle = 0;
        }
      WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
    }
  else
    {
      // We are going to add a new particle and making it the last
      // one, so that the next click would result in a new spring
      current_particle = world.get_particle (x, y);

      if (current_particle)
        {
          // We have clicked on a particle, so make it the current one
        }
      else
        {
          Vector2d new_particle_pos;
          
          if (WorldViewComponent::instance()->uses_grid())
            new_particle_pos = Vector2d(Math::round_to(x, 10),
                                        Math::round_to(y, 10));
          else
            new_particle_pos = Vector2d(x, y);
          
          Particle* p = world.get_particle_mgr()->add_particle(new_particle_pos,
                                                               Vector2d(), 
                                                               particle_mass);
          current_particle = p;
          WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());
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
  World& world = *Controller::instance()->get_world ();

  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x);
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y);

  if (current_particle) 
    { // We are currently creating a new spring, abort that
      current_particle = 0;
      WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
    }
  else
    {
      Spring*   spring   = world.get_spring (x, y);
      Particle* particle = world.get_particle (x, y);

      if (particle)
        {
          Controller::instance()->push_undo();
          world.remove_particle (particle);
        }
      else if (spring)
        {
          Controller::instance()->push_undo();
          world.remove_spring(spring);
        }
    }
}

void
WorldViewInsertTool::on_fix_press (int screen_x, int screen_y) 
{
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x);
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y);

  Particle* particle = Controller::instance()->get_world ()->get_particle (x, y);
  if (particle)
    {
      particle->set_fixed (!particle->get_fixed ());
    }
}

/* EOF */
