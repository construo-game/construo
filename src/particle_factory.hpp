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

#ifndef HEADER_CONSTRUO_PARTICLE_FACTORY_HPP
#define HEADER_CONSTRUO_PARTICLE_FACTORY_HPP

#include <vector>
#include "vector2d.hpp"

#include "lisp_reader.hpp"
#include "lisp_writer.hpp"

class ZoomGraphicContext;
class Particle;
class World;

/** id of a particle, uniq only for a single ParticleFactory */
typedef int ParticleId;

/** Its really more a particle manager, but lets see */
class ParticleFactory
{
public:
  /** Create an empty particle manager */
  ParticleFactory (World*);

  /** Create a particle manager from the data in a .construo file */
  ParticleFactory (World*, ReaderCollection const& particles_collection);

  /** Copy a particle manager, the id's will be keep */
  ParticleFactory (World*, const ParticleFactory&);

  Particle* add_particle (const Vector2d& arg_pos, const Vector2d& arg_velocity,
                          float m, bool f = false);

  /** Duplicate a particle */
  Particle* add_particle (const Particle& particle);

  /** Remove a particle by pointer */
  void remove_particle (Particle*);

  /** Remove a particle by id */
  void remove_particle (int id);

  int size () { return static_cast<int>(m_particles.size()); }
  Particle* lookup_particle (int id);

  void update (float delta);
  void draw (ZoomGraphicContext* gc);

  typedef std::vector<Particle*>::const_iterator CParticleIter;
  typedef std::vector<Particle*>::iterator ParticleIter;

  ParticleIter begin () { return m_particles.begin(); }
  ParticleIter end () { return m_particles.end(); }

  void clear ();
  void write_lisp(LispWriter& writer);

private:
  /** Pointer to the world holding this ParticleManager */
  World* m_world;

  std::vector<Particle*> m_particles;

  /** the id of next particle that will get created */
  int m_particle_id_count;

private:
  ParticleFactory (const ParticleFactory&);
  ParticleFactory& operator= (const ParticleFactory&);
};

#endif

/* EOF */
