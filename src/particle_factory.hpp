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

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "fwd.hpp"

/** id of a particle, uniq only for a single ParticleFactory */
typedef int ParticleId;

/** Its really more a particle manager, but lets see */
namespace construo {

class ParticleFactory
{
public:
  /** Create an empty particle manager */
  ParticleFactory();

  /** Copy a particle manager, the id's will be keep */
  ParticleFactory(const ParticleFactory&);

  Particle* add_particle(const glm::vec2& pos, const glm::vec2& velocity,
                         float mass, bool fixed = false);

  Particle* add_particle(int id, const glm::vec2& pos, const glm::vec2& velocity,
                         float mass, bool fixed = false);

  /** Duplicate a particle */
  Particle* add_particle(const Particle& particle);

  /** Remove a particle by pointer */
  void remove_particle(Particle* particle);

  /** Remove a particle by id */
  void remove_particle(int id);

  int size () { return static_cast<int>(m_particles.size()); }
  Particle* lookup_particle(int id);

  void update(float delta);

  void clear();

  std::vector<std::unique_ptr<Particle>> const& particles() const { return m_particles; }

private:
  std::vector<std::unique_ptr<Particle>> m_particles;

  /** the id of next particle that will get created */
  int m_particle_id_count;

private:
  ParticleFactory& operator=(const ParticleFactory&) = delete;
};

} // namespace construo

#endif

/* EOF */
