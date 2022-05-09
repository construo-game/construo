// Construo - A wire-frame construction game
// Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_CONSTRUO_PARTICLE_HPP
#define HEADER_CONSTRUO_PARTICLE_HPP

#include <iostream>

#include <glm/glm.hpp>

#include "construo.hpp"

class Particle
{
private:
  /** Id of the particle */
   int id;

public:
  /** position of the particle */
  glm::vec2 pos;

  /** velocity of the particle */
  glm::vec2 velocity;

  /** the mass of the particle as 1/mass (FIXME: is this still the
      case?!) */
  float mass;

  /** If true the particle will have a fixed position which cannot be
      changed, if false the particle reacts to forces as normal. */
  bool fixed;

  /** totale force acting on particle (used as temp-var in update() to
      collect the forces)*/
  glm::vec2 totale_force;

  /** Number of connection this particle has to other springs */
  int spring_links;

  /** The id of the particle, used for de/serialisation and copying of
      the World. The id is uniq only for a single world. */
  inline int get_id () const {
    return id;
  }

  void add_force (const glm::vec2& force)
  {
    if (fixed) return;
    totale_force += force;
  }

  void clear_force ()
  {
    totale_force = glm::vec2(0.0f, 0.0f);
  }

  void set_fixed (bool f) {
    fixed = f;
  }

  bool get_fixed () {
    return fixed;
  }

  inline float get_mass () const { return mass; }

  void update (float delta);

  friend class ParticleFactory;

private:
  // Nobody beside the ParticleFactory can create particles
  Particle (int id_, const glm::vec2& arg_pos, const glm::vec2& arg_velocity, float mass_, bool fixed_);
  Particle (const Particle&);
};

#endif

/* EOF */
