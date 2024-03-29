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

#include <string.h>
#include <assert.h>
#include <algorithm>

#include <zlib.h>
#include <geom/rect.hpp>
#include <logmich/log.hpp>

#include "math.hpp"
#include "construo_error.hpp"
#include "world.hpp"
#include "particle_factory.hpp"
#include "system_context.hpp"
#include "controller.hpp"
#include "rect_collider.hpp"

namespace construo {

World::World() :
  m_particle_mgr(std::make_unique<ParticleFactory>()),
  m_springs(),
  m_colliders()
{
}


World::World(const World& other) :
  m_particle_mgr(nullptr),
  m_springs(),
  m_colliders()
{
  for (auto i = other.m_colliders.begin(); i != other.m_colliders.end();
       ++i)
  {
    m_colliders.emplace_back((*i)->duplicate());
  }

  // FIXME: Could need optimizations
  m_particle_mgr = std::make_unique<ParticleFactory>(*other.m_particle_mgr);

  for (auto i = other.m_springs.begin (); i != other.m_springs.end (); ++i)
  {
    Particle* first  = m_particle_mgr->lookup_particle((*i)->particles.first->get_id());
    Particle* second = m_particle_mgr->lookup_particle((*i)->particles.second->get_id());

    if (first && second)
    {
      // FIXME: Use copy c'tor here maxstiffnes and Co. aren't copied correctly
      m_springs.emplace_back(std::make_unique<Spring>(first, second, (*i)->length));
      m_springs.back()->stiffness = (*i)->stiffness;
      m_springs.back()->damping = (*i)->damping;
      m_springs.back()->max_stretch = (*i)->max_stretch;
    }
    else
    {
      log_error("World: Error couldn't resolve particles");
    }
  }
}

World::~World ()
{
}

void
World::update (float delta)
{
  // Main Movement and Forces
  // FIXME: Hardcoded Force Emitters
  for (auto& particle : m_particle_mgr->particles())
  {
    // Gravity
    particle->add_force (glm::vec2 (0.0, 15.0f) * particle->get_mass ());

    // Central Gravity force:
    /*glm::vec2 direction = (particle->pos - glm::vec2 (400, 300));
      if (glm::length(direction) != 0.0f)
      particle->add_force (direction * (-100.0f/(glm::length(direction) * glm::length(direction))));
    */

    /*
      for (auto j = particles.begin (); j != particles.end (); ++j)
      {
      glm::vec2 diff = (*j)->pos - particle->pos;
      if (glm::length(diff) != 0.0f)
      particle->add_force (diff * ((10.0f - (*j)->mass)/(glm::length(diff) * glm::length(diff))));
      }	    */
  }

  for (auto& spring : m_springs) {
    spring->update (delta);
  }

  m_particle_mgr->update(delta);

  for (auto& collider : m_colliders) {
    collider->bounce ();
  }

  // Spring splitting
  std::vector<std::unique_ptr<Spring>> new_springs;
  for (auto& spring : m_springs)
  {
    if (spring->destroyed)
    {
      if (spring->length > 20.0f)
      {
        // Calc midpoint
        glm::vec2 pos = (spring->particles.first->pos
                         + spring->particles.second->pos) * 0.5f;

        // FIXME: particle mass needs to be recalculated
        Particle* p1 = m_particle_mgr->add_particle(pos, spring->particles.first->velocity * 0.5f, .1f);
        Particle* p2 = m_particle_mgr->add_particle(pos, spring->particles.second->velocity * 0.5f, .1f);

        // FIXME: Insert a more sofistikated string splitter here
        new_springs.emplace_back(std::make_unique<Spring>(spring->particles.first, p1, spring->length/2));
        new_springs.emplace_back(std::make_unique<Spring>(spring->particles.second, p2, spring->length/2));
      }
    }
  }
  m_springs.insert(m_springs.end(),
                   std::make_move_iterator(new_springs.begin()),
                   std::make_move_iterator(new_springs.end()));

  std::erase_if(m_springs, [](auto&& spring){ return spring->destroyed; });
}

Spring*
World::find_spring(geom::fpoint const& pos, float capture_distance) const
{
  Spring* found_spring = nullptr;
  float min_distance = 0.0f;

  for (auto const& spring : m_springs)
    {
      float const x0 = pos.x();
      float const y0 = pos.y();
      float const& x1 = spring->particles.first->pos.x;
      float const& y1 = spring->particles.first->pos.y;
      float const& x2 = spring->particles.second->pos.x;
      float const& y2 = spring->particles.second->pos.y;

      // FIXME: optimize me
      float const u = (((x0 - x1) * (x2 - x1) + (y0 - y1) *(y2 - y1)) /
                 ((x2 - x1) * (x2 - x1) + (y2 - y1) *(y2 - y1)));

      float distance = (std::fabs((x2 - x1) * (y1 - y0) - (x1 - x0) * (y2 - y1)) /
                        std::sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));

      if (u >= 0 && u <= 1.0f &&
          ((found_spring && min_distance > distance) ||
           (!found_spring && distance <= capture_distance)))
        {
          found_spring = spring.get();
          min_distance = distance;
        }
    }

  return found_spring;
}

Particle*
World::find_particle(geom::fpoint const& pos, float capture_distance) const
{
  Particle* found_particle = nullptr;
  float min_dist = capture_distance;
  glm::vec2 mouse_pos = pos.as_vec();

  for (auto& particle : m_particle_mgr->particles())
  {
    glm::vec2 diff = mouse_pos - particle->pos;
    if (glm::length(diff) < min_dist)
    {
      min_dist = glm::length(diff);
      found_particle = particle.get();
    }
  }

  return found_particle;
}

Collider*
World::find_collider(geom::fpoint const& pos) const
{
  for (auto it = m_colliders.rbegin (); it != m_colliders.rend(); ++it)
  {
    if ((*it)->is_at(pos.as_vec())) {
      return it->get();
    }
  }
  return nullptr;
}

std::vector<Particle*>
World::find_particles(float x1_, float y1_, float x2_, float y2_) const
{
  float const x1 = std::min(x1_, x2_);
  float const x2 = std::max(x1_, x2_);
  float const y1 = std::min(y1_, y2_);
  float const y2 = std::max(y1_, y2_);

  std::vector<Particle*> caputred_particles;
  for (auto& particle : m_particle_mgr->particles())
  {
    if (particle->pos.x >= x1 && particle->pos.x < x2 &&
        particle->pos.y >= y1 && particle->pos.y < y2) {
      caputred_particles.push_back(particle.get());
    }
  }
  return caputred_particles;
}

Spring*
World::add_spring(int lhs, int rhs, float length, float stiffness, float damping, float max_stretch)
{
  Particle* lhs_particle = m_particle_mgr->lookup_particle(lhs);
  Particle* rhs_particle = m_particle_mgr->lookup_particle(rhs);
  if (lhs_particle == nullptr ||
      rhs_particle == nullptr)
  {
    throw std::runtime_error("spring: particle lookup failed");
  }

  if (length == -1)
  {
    log_warn("World: spring length missing, recalculating");
    length = std::fabs(glm::length(lhs_particle->pos - rhs_particle->pos));
  }

  lhs_particle->spring_links += 1;
  rhs_particle->spring_links += 1;

  m_springs.emplace_back(std::make_unique<Spring>(lhs_particle, rhs_particle));

  m_springs.back()->stiffness = stiffness;
  m_springs.back()->damping = damping;
  m_springs.back()->max_stretch = max_stretch;

  return m_springs.back().get();
}

Spring*
World::add_spring(Particle* last_particle, Particle* particle, float stiffness)
{
  assert(last_particle && particle);
  m_springs.emplace_back(std::make_unique<Spring>(last_particle, particle));
  m_springs.back()->stiffness = stiffness;

  // FIXME: math this properly
  m_springs.back()->max_stretch = 0.15f * 50.0f / stiffness;

  return m_springs.back().get();
}

void
World::add_particle(int id, glm::vec2 const& pos, glm::vec2 const& velocity, float mass, bool fixed)
{
  m_particle_mgr->add_particle(id, pos, velocity, mass, fixed);
}

void
World::remove_particle(Particle* particle)
{
  // Remove everyting that references the particle
  std::erase_if(m_springs, [particle](auto&& spring) {
    return (spring->particles.first == particle ||
            spring->particles.second == particle);
  });

  m_particle_mgr->remove_particle(particle);
}

void
World::remove_spring(Spring* s)
{
  std::erase_if(m_springs, [s](auto&& spring) { return spring.get() == s; });
}

void
World::remove_collider(Collider* c)
{
  std::erase_if(m_colliders, [c](auto&& collider){ return collider.get() == c; });
}

geom::frect
World::calc_bounding_box() const
{
  geom::frect bbox;

  if (m_particle_mgr->particles().empty())
  {
    bbox = geom::frect(m_particle_mgr->particles().front()->pos,
                       geom::fsize(1, 1));
  }
  else
  {
    bbox = geom::frect(0.0f, 0.0f, 800.0f, 600.0f);
  }

  for (auto& particle : m_particle_mgr->particles())
  {
    bbox = geom::frect(std::min(bbox.left(), particle->pos.x),
                       std::min(bbox.top(), particle->pos.y),
                       std::max(bbox.right(), particle->pos.x),
                       std::max(bbox.bottom(), particle->pos.y));

  }

  for (auto& collider : m_colliders)
  {
    bbox = geom::unite(bbox, collider->get_bounding_box());
  }

  return bbox;
}

void
World::add_rect_collider(const glm::vec2& pos1, const glm::vec2& pos2)
{
  geom::frect rect(pos1, pos2);

  m_colliders.emplace_back(std::make_unique<RectCollider>(rect.left(), rect.top(), rect.right(), rect.bottom()));
}

} // namespace construo

/* EOF */
