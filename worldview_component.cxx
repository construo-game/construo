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

#include "controller.hxx"
#include "particle_factory.hxx"
#include "worldview_component.hxx"

extern Controller* controller;

WorldViewComponent::WorldViewComponent ()
{
}

WorldViewComponent::~WorldViewComponent ()
{
  
}

void
WorldViewComponent::draw (GraphicContext* gc)
{
  controller->get_world()->draw (gc);
}

void
WorldViewComponent::on_primary_button_click (int x, int y)
{
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
WorldViewComponent::on_delete_press (int x, int y)
{
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
WorldViewComponent::on_fix_press (int x, int y)
{
  Particle* particle = controller->get_world ()->get_particle (x, y);
  std::cout << "Fixing particle: " << particle << std::endl;
  if (particle)
    {
      std::cout << "particle: " << particle->get_fixed () << std::endl;
      particle->set_fixed (!particle->get_fixed ());
    }
}

/* EOF */
