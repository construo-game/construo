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

#ifndef HEADER_CONSTRUO_SPRING_HXX
#define HEADER_CONSTRUO_SPRING_HXX

#include <iostream>
#include <assert.h>
#include "particle.hxx"
#include "world.hxx"
#include "lisp_reader.hxx"

class World;

class Spring
{
public:
  std::pair<Particle*, Particle*> particles;
  
  float length;
  bool destroyed;

  Spring (Particle* f, Particle* s, float l) {
    particles.first = f;
    particles.second = s;
    destroyed = false;
    length = l;
  }

  Spring (Particle* f, Particle* s) {
    particles.first = f;
    particles.second = s;
    destroyed = false;
    length = fabs((f->pos - s->pos).norm ());

    assert (length != 0);
  }

  Spring (World* world, lisp_object_t* cursor);

  void update (float delta);
  void draw ();

  lisp_object_t* serialize()
  {
    LispWriter obj ("spring");
    obj.write_int ("first", particles.first->get_id());
    obj.write_int ("second", particles.second->get_id());
    obj.write_float ("length", length);
    return obj.get_lisp ();
  }
};

#endif

/* EOF */
