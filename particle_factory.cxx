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

#include "lisp_reader.hxx"
#include "zoom_graphic_context.hxx"
#include "particle.hxx"
#include "construo_error.hxx"
#include "particle_factory.hxx"

ParticleFactory::ParticleFactory ()
  : particle_id_count(0)
{
}

ParticleFactory::ParticleFactory (lisp_object_t* cursor)
  : particle_id_count (0)
{
  while(!lisp_nil_p(cursor))
    {
      lisp_object_t* obj = lisp_car(cursor);
      Vector2d pos;
      Vector2d velocity;
      float mass = 15.0f;
      bool fixed = false;
      int id = -1;
            
      obj = lisp_cdr(obj); // skip particle 'marker'

      LispReader reader(obj);
      reader.read_vector ("pos", &pos);
      reader.read_vector ("velocity", &velocity);
      reader.read_float ("mass", &mass);
      reader.read_bool ("fixed", &fixed);
      reader.read_int ("id", &id);

      if (id >= particle_id_count)
        particle_id_count = id + 1;

      particles.push_back(new Particle (id, pos, velocity, mass, fixed));

      cursor = lisp_cdr (cursor);
    }
}

ParticleFactory::ParticleFactory (const ParticleFactory& pmgr)
{
  particle_id_count = pmgr.particle_id_count;
  for (CParticleIter i = pmgr.particles.begin (); i != pmgr.particles.end (); ++i)
    particles.push_back(new Particle(**i));
}

ParticleFactory&
ParticleFactory::operator= (const ParticleFactory& pmgr)
{  
  ConstruoAssert (0, "Don't use this");
  for (CParticleIter i = pmgr.particles.begin ();
       i != pmgr.particles.end (); 
       ++i)
    {
      particles.push_back (new Particle (*(*i)));
    }
  return *this;
}

Particle*
ParticleFactory::add_particle (const Vector2d& arg_pos, const Vector2d& arg_velocity, float m, bool f)
{
  Particle* p = new Particle(particle_id_count++,
                             arg_pos, 
                             arg_velocity, m, f);
  particles.push_back(p);
  return p;
}

void
ParticleFactory::remove_particle (Particle* p)
{
  // Remove the particle itself
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      if (*i == p)
        {
          delete *i;
          particles.erase(i);
          return;
        }
    }
}

void
ParticleFactory::update (float delta)
{
  for (CParticleIter i = particles.begin (); i != particles.end (); ++i)
    (*i)->update(delta);
}

void
ParticleFactory::draw (ZoomGraphicContext* gc)
{
  for (CParticleIter i = particles.begin (); i != particles.end (); ++i)
    (*i)->draw(gc);
}

Particle*
ParticleFactory::lookup_particle (int id)
{
  // FIXME: Could need optimization
  for (ParticleIter i = particles.begin (); 
       i != particles.end (); 
       ++i)
    {
      if ((*i)->get_id () == id)
        return *i;
    }
  return 0;
}

void
ParticleFactory::clear ()
{
 for (CParticleIter i = particles.begin (); i != particles.end (); ++i)
   delete *i;
 particles.clear ();
}

void
ParticleFactory::write_lisp(FILE* out)
{
  fputs("  (particles\n", out);
  for (CParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      lisp_object_t* obj = (*i)->serialize ();
      fputs("    ", out);
      lisp_dump (obj, out);
      lisp_free(obj);
      fputc('\n', out);
    }
  fputs("  )\n", out);
}

/* EOF */
