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

#include "construo_error.hxx"
#include "particle_factory.hxx"
#include "spring.hxx"

Spring::Spring (Particle* f, Particle* s, float l)
{
  particles.first  = f;
  particles.second = s;
  destroyed        = false;
  length           = l;
}

Spring::Spring (Particle* f, Particle* s) 
{
  particles.first  = f;
  particles.second = s;
  destroyed        = false;
  length           = fabs((f->pos - s->pos).norm ());

  assert (length != 0);
}

Spring::Spring (World* world, lisp_object_t* cursor)
  : destroyed (false)
{
  cursor = lisp_cdr(cursor); // Skip the identifer
  
  int first_id = -1;
  int second_id = -1;
  length = -1;

  LispReader reader(cursor);
  reader.read_int ("first", &first_id);
  reader.read_int ("second", &second_id);
  reader.read_float ("length", &length);

  particles.first  = world->get_particle_mgr()->lookup_particle (first_id);
  particles.second = world->get_particle_mgr()->lookup_particle (second_id);

  if (particles.first == 0 || particles.second == 0)
    {
      throw ConstruoError ("Spring: Pair lookup failed");
    }

  if (length == -1)
    {
      std::cout << "Spring: length missing in data file, recalculating" << std::endl;
      length = fabs((particles.first->pos - particles.second->pos).norm ());
    }
}

void
Spring::update (float delta)
{
  Vector2d dist = particles.first->pos - particles.second->pos;
  float stretch = dist.norm ()/length - 1.0f;
  stretch *= 2.0f; // Materialkoeffizent
  //std::cout << "stretch: " << stretch << std::endl;

  if (fabs(stretch) > max_stretch && 
      length > 10.0f) // atomar spring
    {
      destroyed = true;
    }
  else
    {
      dist.normalize ();
      Vector2d force = dist * stretch * back_force;
      //std::cout << "Force: " << force << std::endl;
      particles.first->add_force (-force);
      particles.second->add_force (force);
    }
}

void
Spring::draw (GraphicContext* gc)
{
  Vector2d dist = particles.first->pos - particles.second->pos;
  float stretch = fabs(dist.norm ()/length - 1.0f) * 10.0f;
  
  float color = fabs((stretch/max_stretch));
  
  gc->draw_line (int(particles.first->pos.x), int(particles.first->pos.y),
                 int(particles.second->pos.x), int(particles.second->pos.y),
                 Color(color, 1.0f - color, 0.0f));
}

void
Spring::draw_highlight (GraphicContext* gc)
{
  gc->draw_line (int(particles.first->pos.x), int(particles.first->pos.y),
                 int(particles.second->pos.x), int(particles.second->pos.y),
                 Color(0xEEEEEE), 
                 10);
}


lisp_object_t* 
Spring::serialize()
{
  LispWriter obj ("spring");
  obj.write_int ("first", particles.first->get_id());
  obj.write_int ("second", particles.second->get_id());
  obj.write_float ("length", length);
  return obj.get_lisp ();
}

/* EOF */
