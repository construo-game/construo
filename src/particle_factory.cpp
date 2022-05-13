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

#include <float.h>
#include <algorithm>
#include "zoom_graphic_context.hpp"
#include "particle.hpp"
#include "construo_error.hpp"
#include "world.hpp"
#include "particle_factory.hpp"

ParticleFactory::ParticleFactory() :
  m_particles(),
  m_particle_id_count(0)
{
}

ParticleFactory::ParticleFactory(const ParticleFactory& pmgr) :
  m_particles(),
  m_particle_id_count(pmgr.m_particle_id_count)
{
  for (auto& particle : pmgr.m_particles) {
    m_particles.emplace_back(std::make_unique<Particle>(*particle));
  }
}

Particle*
ParticleFactory::add_particle(int id, const glm::vec2& pos, const glm::vec2& velocity,
                              float mass, bool fixed)
{
  if (id >= m_particle_id_count) {
    m_particle_id_count = id + 1;
  }

  m_particles.emplace_back(std::make_unique<Particle>(id, pos, velocity, mass, fixed));
  return m_particles.back().get();
}

Particle*
ParticleFactory::add_particle (const glm::vec2& pos, const glm::vec2& velocity, float mass, bool fixed)
{
  return add_particle(m_particle_id_count++, pos, velocity, mass, fixed);
}

Particle*
ParticleFactory::add_particle (const Particle& particle)
{
  auto p = std::make_unique<Particle>(particle);
  p->id = m_particle_id_count++;
  m_particles.emplace_back(std::move(p));
  return m_particles.back().get();
}

void
ParticleFactory::remove_particle(Particle* p)
{
  std::erase_if(m_particles, [p](auto&& particle){ return particle.get() == p; });
}

void
ParticleFactory::update (float delta)
{
  for (auto i = m_particles.begin (); i != m_particles.end (); ++i)
    (*i)->update(delta);

  // FIXME: There is no need to do this on any update, doing it only
  //once a second should be enough
  std::erase_if(m_particles, [](auto&& p) {
    return (p->spring_links == 0
            && p->velocity.x == 0
            && std::fabs(p->velocity.y) < 0.1f);
  });
}

Particle*
ParticleFactory::lookup_particle (int id)
{
  // FIXME: Could need optimization
  for (auto i = m_particles.begin(); i != m_particles.end(); ++i)
    {
      if ((*i)->get_id () == id)
        return (*i).get();
    }
  return nullptr;
}

void
ParticleFactory::clear ()
{
  m_particles.clear ();
}

/* EOF */
