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

#include "math.hpp"
#include "construo_error.hpp"
#include "world.hpp"
#include "particle_factory.hpp"
#include "system_context.hpp"
#include "controller.hpp"
#include "rect_collider.hpp"
#include "string_utils.hpp"

World::World() :
  m_file_version(0),
  m_has_been_run(false),
  m_particle_mgr(std::make_unique<ParticleFactory>()),
  m_springs(),
  m_colliders()
{
}

World::World(const std::string& filename) :
  m_file_version(0),
  m_has_been_run(false),
  m_particle_mgr(nullptr),
  m_springs(),
  m_colliders()
{
  std::cout << "World: Loading '" << filename << "'..." << std::endl;
  m_file_version = 0;

  m_has_been_run = false;

  ReaderDocument doc = ReaderDocument::from_file(g_system_context->translate_filename(filename));
  if (doc.get_name() != "construo-scene") {
    throw ConstruoError ("World: Read error in " + filename + ". Couldn't find 'construo-scene'");
  }

  parse_scene(doc.get_mapping());

  ConstruoAssert(m_particle_mgr, "No Particles given in file, load failed");

  //std::cout << "particles: " << particle_mgr->size () << std::endl;
  //std::cout << "springs:   " << springs.size () << std::endl;
}

// Copy Constructor
World::World(const World& old_world) :
  m_file_version(0),
  m_has_been_run(false),
  m_particle_mgr(nullptr),
  m_springs(),
  m_colliders()
{
  for (Colliders::const_iterator i = old_world.m_colliders.begin();
       i != old_world.m_colliders.end();
       ++i)
  {
    m_colliders.push_back((*i)->duplicate());
  }

  // FIXME: Could need optimizations
  m_particle_mgr = std::make_unique<ParticleFactory>(*old_world.m_particle_mgr);

  for (auto i = old_world.m_springs.begin (); i != old_world.m_springs.end (); ++i)
  {
    Particle* first  = m_particle_mgr->lookup_particle((*i)->particles.first->get_id());
    Particle* second = m_particle_mgr->lookup_particle((*i)->particles.second->get_id());

    if (first && second)
    {
      // FIXME: Use copy c'tor here maxstiffnes and Co. aren't copied correctly
      m_springs.push_back(new Spring (first, second, (*i)->length));
    }
    else
    {
      std::cout << "World: Error couldn't resolve particles" << std::endl;
    }
  }
}

World::~World ()
{
  clear();
}

void
World::parse_scene(ReaderMapping const& reader)
{
  reader.read("version", m_file_version);

  if (ReaderCollection particles = reader.get<ReaderCollection>("particles")) {
    parse_particles(particles);
  }

  if (ReaderCollection springs = reader.get<ReaderCollection>("springs")) {
    parse_springs(springs);
  }

  if (ReaderCollection colliders = reader.get<ReaderCollection>("colliders")) {
    parse_colliders(colliders);
  }
}

void
World::parse_springs(ReaderCollection const& collection)
{
  for (ReaderObject const& item : collection.get_objects()) {
    m_springs.push_back(new Spring(this, item.get_mapping()));
  }
}

void
World::parse_colliders(ReaderCollection const& collection)
{
  for(auto const& item : collection.get_objects()) {
    if (item.get_name() == "rect") {
      m_colliders.push_back(new RectCollider(item.get_mapping()));
    } else {
      std::cout << "WARNING: Unknown collider type '" << item.get_name()
                << "' skipping" << std::endl;
    }
  }
}

void
World::parse_particles(ReaderCollection const& collection)
{
  m_particle_mgr = std::make_unique<ParticleFactory>(m_file_version, collection);
}

void
World::update (float delta)
{
  m_has_been_run = true;

  // Main Movement and Forces
  // FIXME: Hardcoded Force Emitters
  for (auto i = m_particle_mgr->begin (); i != m_particle_mgr->end (); ++i)
    {
      // Gravity
      (*i)->add_force (glm::vec2 (0.0, 15.0f) * (*i)->get_mass ());

      // Central Gravity force:
      /*glm::vec2 direction = ((*i)->pos - glm::vec2 (400, 300));
        if (glm::length(direction) != 0.0f)
        (*i)->add_force (direction * (-100.0f/(glm::length(direction) * glm::length(direction))));
      */

      /*
        for (auto j = particles.begin (); j != particles.end (); ++j)
        {
        glm::vec2 diff = (*j)->pos - (*i)->pos;
        if (glm::length(diff) != 0.0f)
        (*i)->add_force (diff * ((10.0f - (*j)->mass)/(glm::length(diff) * glm::length(diff))));
        }	    */
    }

  for (auto i = m_springs.begin (); i != m_springs.end (); ++i)
    (*i)->update (delta);

  m_particle_mgr->update(delta);

  //std::cout << "Colliders: " << colliders.size () << std::endl;
  for (auto i = m_colliders.begin (); i != m_colliders.end (); ++i)
    (*i)->bounce ();

  // Spring splitting
  std::vector<Spring*> new_springs;
  for (auto i = m_springs.begin (); i != m_springs.end (); ++i)
    {
      if ((*i)->destroyed)
        {
          if ((*i)->length > 20.0f)
            {
              // Calc midpoint
              glm::vec2 pos = ((*i)->particles.first->pos
                               + (*i)->particles.second->pos) * 0.5f;

              // FIXME: particle mass needs to be recalculated
              Particle* p1 = m_particle_mgr->add_particle (pos, (*i)->particles.first->velocity * 0.5f, .1f);
              Particle* p2 = m_particle_mgr->add_particle (pos, (*i)->particles.second->velocity * 0.5f, .1f);

              // FIXME: Insert a more sofistikated string splitter here
              new_springs.push_back(new Spring ((*i)->particles.first, p1, (*i)->length/2));
              new_springs.push_back(new Spring ((*i)->particles.second, p2, (*i)->length/2));
            }
        }
    }
  m_springs.insert(m_springs.end(), new_springs.begin(), new_springs.end ());

  // Remove any springs that are marked as destroyed
  // FIXME: Could be faster
  SpringIter i = m_springs.begin ();
  while ( i != m_springs.end ())
    {
      if ((*i)->destroyed)
        {
          delete *i;
          i = m_springs.erase(i);
        }
      else
        {
          ++i;
        }
    }
}

Spring*
World::get_spring(float x, float y, float capture_distance) const
{
  Spring* spring = nullptr;
  float min_distance = 0.0f;

  for (auto i = m_springs.begin (); i != m_springs.end (); ++i)
    {
      float x0 = x;
      float y0 = y;
      float& x1 = (*i)->particles.first->pos.x;
      float& y1 = (*i)->particles.first->pos.y;
      float& x2 = (*i)->particles.second->pos.x;
      float& y2 = (*i)->particles.second->pos.y;

      // FIXME: optimize me
      float u = (((x0 - x1)*(x2-x1) + (y0 - y1)*(y2 - y1))
                 / ((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));

      float distance = (std::fabs((x2 - x1)*(y1-y0) - (x1-x0)*(y2-y1))
                        / std::sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));

      if (u >= 0 && u <= 1.0f
          && ((spring && min_distance > distance)
              || (!spring && distance <= capture_distance)))
        {
          spring = *i;
          min_distance = distance;
        }
    }

  return spring;
}

Particle*
World::get_particle(float x, float y, float capture_distance) const
{
  Particle* particle = nullptr;
  float min_dist = capture_distance;
  glm::vec2 mouse_pos (x, y);

  for (auto i = m_particle_mgr->begin (); i != m_particle_mgr->end (); ++i)
    {
      glm::vec2 diff = mouse_pos - (*i)->pos;
      if (glm::length(diff) < min_dist)
	{
	  min_dist = glm::length(diff);
	  particle = *i;
	}
    }

  return particle;
}

std::vector<Particle*>
World::get_particles(float x1_, float y1_, float x2_, float y2_) const
{
  float const x1 = std::min(x1_, x2_);
  float const x2 = std::max(x1_, x2_);
  float const y1 = std::min(y1_, y2_);
  float const y2 = std::max(y1_, y2_);

  std::vector<Particle*> caputred_particles;
  for (auto i = m_particle_mgr->begin (); i != m_particle_mgr->end (); ++i)
    {
      if ((*i)->pos.x >= x1 && (*i)->pos.x < x2
          && (*i)->pos.y >= y1 && (*i)->pos.y < y2)
        caputred_particles.push_back(*i);
    }
  return caputred_particles;
}

void
World::zero_out_velocity ()
{
  std::cout << "Setting velocity to zero" << std::endl;
  for (auto i = get_particle_mgr().begin();
       i != get_particle_mgr().end(); ++i)
    {
      (*i)->velocity = glm::vec2(0.0f, 0.0f);
    }
}

void
World::add_spring (Particle* last_particle, Particle* particle)
{
  assert (last_particle && particle);
  m_springs.push_back(new Spring(last_particle, particle));
}

void
World::remove_particle (Particle* p)
{
  // Remove everyting that references the particle
  for (auto i = m_springs.begin (); i != m_springs.end ();)
    {
      if ((*i)->particles.first == p || (*i)->particles.second == p)
        {
          delete *i;
          // FIXME: this is potentially slow, since we don't care
          // about order, we could speed this up
          i = m_springs.erase(i);
        }
      else
        {
          ++i;
        }
    }

  m_particle_mgr->remove_particle(p);
}

void
World::remove_spring (Spring* s)
{
  //std::cout << "particles: " << particle_mgr->size () << std::endl;
  //std::cout << "springs:   " << springs.size () << std::endl;

  delete s;
  m_springs.erase(std::remove(m_springs.begin (), m_springs.end (), s),
                  m_springs.end ());
}

void
World::remove_collider (Collider* c)
{
  delete c;
  m_colliders.erase(std::remove(m_colliders.begin(), m_colliders.end(), c),
                    m_colliders.end ());
}

void
World::clear ()
{
  m_particle_mgr->clear();

  for (auto i = m_springs.begin (); i != m_springs.end (); ++i)
    delete *i;

  m_springs.clear();
}

void
World::write_lisp (const std::string& filename)
{
  std::cout << "World: Writing to: " << filename << std::endl;

  prio::Writer writer = prio::Writer::from_file(g_system_context->translate_filename(filename));

  writer.write_comment("Written by " PACKAGE_STRING);
  writer.begin_object("construo-scene");
  writer.write("version",  3);
  writer.write("author", std::vector<std::string>({
        g_system_context->get_user_realname(),
        g_system_context->get_user_email()}));

  m_particle_mgr->write_lisp(writer);

  writer.begin_collection("springs");
  for (auto i = m_springs.begin (); i != m_springs.end (); ++i) {
    (*i)->serialize(writer);
  }
  writer.end_collection();

  writer.begin_collection("colliders");
  for (auto i = m_colliders.begin(); i != m_colliders.end(); ++i) {
    (*i)->serialize (writer);
  }
  writer.end_collection();
  writer.end_object();
}

BoundingBox
World::calc_bounding_box() const
{
  BoundingBox bbox;

  if (m_particle_mgr->size() > 0)
  {
    bbox.x1 = bbox.x2 = (*m_particle_mgr->begin())->pos.x;
    bbox.y1 = bbox.y2 = (*m_particle_mgr->begin())->pos.y;
  }
  else
  {
    bbox.x1 = 0;
    bbox.y1 = 0;

    bbox.x2 = 800;
    bbox.y2 = 600;
  }

  for (auto i = m_particle_mgr->begin(); i != m_particle_mgr->end(); ++i)
  {
    bbox.join((*i)->pos);
  }

  for (auto i = m_colliders.begin(); i != m_colliders.end(); ++i)
  {
    bbox.join((*i)->get_bounding_box());
  }

  return bbox;
}

int
World::get_num_particles()
{
  return m_particle_mgr->size ();
}

int
World::get_num_springs()
{
  return static_cast<int>(m_springs.size());
}

void
World::add_rect_collider(const glm::vec2& pos1, const glm::vec2& pos2)
{
  geom::frect rect(pos1, pos2);

  m_colliders.push_back(new RectCollider(rect.left(), rect.top(), rect.right(), rect.bottom()));
}

/* EOF */
