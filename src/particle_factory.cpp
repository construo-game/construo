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
#include "lisp_reader.hpp"
#include "zoom_graphic_context.hpp"
#include "particle.hpp"
#include "construo_error.hpp"
#include "world.hpp"
#include "particle_factory.hpp"

ParticleFactory::ParticleFactory (World* w) :
  m_world (w),
  m_particles(),
  m_particle_id_count(0)
{
}

ParticleFactory::ParticleFactory (World* w, ReaderCollection const& collection) :
  m_world(w),
  m_particles(),
  m_particle_id_count(0)
{
  for(ReaderObject const& item : collection.get_objects()) {
    ReaderMapping const& reader = item.get_mapping();

    Vector2d pos;
    Vector2d velocity;
    float mass = 1.0f/10.0f;
    bool fixed = false;
    int id = -1;

    reader.read("pos", pos);
    reader.read("velocity", velocity);
    reader.read("mass", mass);
    reader.read("fixed", fixed);
    reader.read("id", id);

    switch (m_world->m_file_version) {
      case 0:
      case 1:
      case 2:
        mass = 1.0f/10.0f;
        break;
    }

    if (id >= m_particle_id_count) {
      m_particle_id_count = id + 1;
    }

    m_particles.push_back(new Particle (id, pos, velocity, mass, fixed));
  }
}

ParticleFactory::ParticleFactory (World* w, const ParticleFactory& pmgr) :
  m_world (w),
  m_particles(),
  m_particle_id_count(pmgr.m_particle_id_count)
{
  for (CParticleIter i = pmgr.m_particles.begin (); i != pmgr.m_particles.end (); ++i)
    m_particles.push_back(new Particle(**i));
}

ParticleFactory&
ParticleFactory::operator= (const ParticleFactory& pmgr)
{
  ConstruoAssert (0, "Don't use this");
  for (CParticleIter i = pmgr.m_particles.begin ();
       i != pmgr.m_particles.end ();
       ++i)
    {
      m_particles.push_back (new Particle (*(*i)));
    }
  return *this;
}

Particle*
ParticleFactory::add_particle (const Vector2d& arg_pos, const Vector2d& arg_velocity, float m, bool f)
{
  Particle* p = new Particle(m_particle_id_count++,
                             arg_pos,
                             arg_velocity, m, f);
  m_particles.push_back(p);
  return p;
}

Particle*
ParticleFactory::add_particle (const Particle& particle)
{
  Particle* p = new Particle (particle);
  p->id = m_particle_id_count++,
  m_particles.push_back(p);
  return p;
}

void
ParticleFactory::remove_particle (Particle* p)
{
  // Remove the particle itself
  for (ParticleIter i = m_particles.begin (); i != m_particles.end (); ++i)
    {
      if (*i == p)
        {
          delete *i;
          m_particles.erase(i);
          return;
        }
    }
}

struct particle_obsolete
{
  inline bool operator()(Particle* p)
  {
    return (p->spring_links == 0
            && p->velocity.x == 0
            && fabsf(p->velocity.y) < 0.1f);
  }
};

void
ParticleFactory::update (float delta)
{
  for (CParticleIter i = m_particles.begin (); i != m_particles.end (); ++i)
    (*i)->update(delta);

  // FIXME: There is no need to do this on any update, doing it only
  //once a second should be enough
  m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(),
                                   particle_obsolete()),
                    m_particles.end());
}

void
ParticleFactory::draw (ZoomGraphicContext& gc)
{
  for (CParticleIter i = m_particles.begin (); i != m_particles.end (); ++i)
    (*i)->draw(gc);
}

Particle*
ParticleFactory::lookup_particle (int id)
{
  // FIXME: Could need optimization
  for (ParticleIter i = m_particles.begin ();
       i != m_particles.end ();
       ++i)
    {
      if ((*i)->get_id () == id)
        return *i;
    }
  return nullptr;
}

void
ParticleFactory::clear ()
{
  for (CParticleIter i = m_particles.begin (); i != m_particles.end (); ++i)
    delete *i;
  m_particles.clear ();
}

void
ParticleFactory::write_lisp(LispWriter& writer)
{
  writer.begin_collection("particles");
  for (CParticleIter i = m_particles.begin (); i != m_particles.end (); ++i) {
    (*i)->serialize(writer);
  }
  writer.end_collection();
}

/* EOF */
