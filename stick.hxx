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

#ifndef HEADER_CONSTRUO_STICK_HXX
#define HEADER_CONSTRUO_STICK_HXX

#include <iostream>
#include <assert.h>
#include "particle.hxx"
#include "world.hxx"
#include "lisp_reader.hxx"

class World;

class Stick
{
public:
  std::pair<Particle*, Particle*> particles;
  
  float length;
  bool destroyed;

  Stick (Particle* f, Particle* s, float l) {
    particles.first = f;
    particles.second = s;
    destroyed = false;
    length = l;
  }

  Stick (Particle* f, Particle* s) {
    particles.first = f;
    particles.second = s;
    destroyed = false;
    length = fabs((f->pos - s->pos).norm ());

    assert (length != 0);
  }

  Stick (World* world, lisp_object_t* cursor);

  void write_xml (std::ostream& out)
  {
    out << "    <spring>"
	<< "<first>" << particles.first->get_id () << "</first>"
	<< "<second>" << particles.second->get_id () << "</second>"
	<< "</spring>" << std::endl;
  }
  
  void update (float delta)
  {
    CL_Vector dist = particles.first->pos - particles.second->pos;
    float stretch = dist.norm ()/length - 1.0f;
    stretch *= 2.0f; // Materialkoeffizent
    //std::cout << "stretch: " << stretch << std::endl;

    if (fabs(stretch) > max_stretch)
      {
	destroyed = true;
      }
    else
      {
	dist.normalize ();
	CL_Vector force = dist * stretch * back_force;
	//std::cout << "Force: " << force << std::endl;
	particles.first->add_force (-force);
	particles.second->add_force (force);
      }
  }
  
  void draw ()
  {
    CL_Vector dist = particles.first->pos - particles.second->pos;
    float stretch = fabs(dist.norm ()/length - 1.0f) * 10.0f;

    float color = fabs((stretch/max_stretch));

    graphic_context->draw_line (int(particles.first->pos.x), int(particles.first->pos.y),
                                int(particles.second->pos.x), int(particles.second->pos.y),
                                Color(color, 1.0f - color, 0.0f));
  }

  lisp_object_t* serialize()
  {
    Lispifier obj ("spring");
    obj.write_int ("first", particles.first->get_id());
    obj.write_int ("second", particles.second->get_id());
    obj.write_float ("length", length);
    return obj.get_lisp ();
  }
};

#endif

/* EOF */
