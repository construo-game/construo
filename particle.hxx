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

#ifndef HEADER_CONSTRUO_PARTICLE_HXX
#define HEADER_CONSTRUO_PARTICLE_HXX

#include <iostream>
#include "lisp_writer.hxx"
#include "construo.hxx"
#include "graphic_context.hxx"
#include "vector2d.hxx"

class Particle
{
private:
  /** Id of the particle */
   int id;

public:
  /** position of the particle */
  Vector2d pos;

  /** velocity of the particle */
  Vector2d  velocity;

  /** the mass of the particle as 1/mass */
  float mass;

  bool fixed;

  /** totale force acting on particle (used as temp-var in update() to
      collect the forces)*/
  Vector2d totale_force;

  /** The id of the particle, used for de/serialisation and copying of
      the World. The id is uniq only for a single world. */
  int get_id () {
    return id;
  }

  void add_force (const Vector2d& force)
  {
    if (fixed) return;
    totale_force += force;
  }

  void clear_force ()
  {
    totale_force = Vector2d ();
  }

  void set_fixed (bool f) {
    fixed = f;
  }

  bool get_fixed () {
    return fixed;
  }  

  void update (float delta); 
  void draw (GraphicContext* gc);

  /** draws the particle in highlight mode (aka if mouse is over it) */
  void draw_highlight (GraphicContext* gc);

  lisp_object_t* serialize();

  friend class ParticleFactory;

private:
  // Nobody beside the ParticleFactory can create particles
  Particle (int id_, const Vector2d& arg_pos, const Vector2d& arg_velocity, float mass_, bool fixed_);
  Particle (const Particle&);
};

#endif

/* EOF */
