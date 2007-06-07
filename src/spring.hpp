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

#ifndef HEADER_CONSTRUO_SPRING_HPP
#define HEADER_CONSTRUO_SPRING_HPP

#include <iostream>
#include <assert.h>
#include "particle.hpp"
#include "world.hpp"
#include "lisp_reader.hpp"

class World;

class Spring
{
public:
  /** Pointers to the two particles to which the Spring is
      connected */
  std::pair<Particle*, Particle*> particles;
  
  /** The orignial length of the Spring, the actually length on the
      screen is different, since the spring can be streched */
  float length;
  
  /** */
  bool destroyed;

  float stiffness;
  float damping;
  float max_stretch;

  Spring (Particle* f, Particle* s, float l);
  Spring (Particle* f, Particle* s);

  Spring (World* world, lisp_object_t* cursor);

  ~Spring ();

  void update (float delta);
  void draw (ZoomGraphicContext* gc);
  void draw_highlight (ZoomGraphicContext* gc);

  /** Forces the recalculation of the springs length */
  void recalc_length ();

  lisp_object_t* serialize();
};

#endif

/* EOF */
