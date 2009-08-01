//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_CONSTRUO_PARTICLE_HPP
#define HEADER_CONSTRUO_PARTICLE_HPP

#include <iostream>
#include "lisp_writer.hpp"
#include "construo.hpp"
#include "zoom_graphic_context.hpp"
#include "vector2d.hpp"

class Particle
{
private:
  /** Id of the particle */
   int id;

public:
  /** position of the particle */
  Vector2d pos;

  /** velocity of the particle */
  Vector2d velocity;

  /** the mass of the particle as 1/mass (FIXME: is this still the
      case?!) */
  float mass;

  /** If true the particle will have a fixed position which cannot be
      changed, if false the particle reacts to forces as normal. */
  bool fixed;

  /** totale force acting on particle (used as temp-var in update() to
      collect the forces)*/
  Vector2d totale_force;

  /** Number of connection this particle has to other springs */
  int spring_links;

  /** The id of the particle, used for de/serialisation and copying of
      the World. The id is uniq only for a single world. */
  inline int get_id () const {
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

  inline float get_mass () const { return mass; }

  void update (float delta); 
  void draw (ZoomGraphicContext* gc);

  /** draws the particle in highlight mode (aka if mouse is over it) */
  void draw_highlight (ZoomGraphicContext* gc);
  
  /** draws additional infos to this stop */
  void draw_infos (ZoomGraphicContext* gc);

  void draw_velocity_vector (ZoomGraphicContext* gc);

  lisp_object_t* serialize();

  friend class ParticleFactory;

private:
  // Nobody beside the ParticleFactory can create particles
  Particle (int id_, const Vector2d& arg_pos, const Vector2d& arg_velocity, float mass_, bool fixed_);
  Particle (const Particle&);
};

#endif

/* EOF */
