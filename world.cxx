//  $Id$
//
//  Pingus - A free Lemmings clone
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

#include "config.h"
#include "construo_error.hxx"
#include "world.hxx"

bool stick_destroyed (Stick* stick)
{
  return stick->destroyed;
}

World::World ()
{
}

World::World (const std::string& filename)
{
  FILE* in;
  lisp_stream_t stream;

  in = fopen(filename.c_str(), "r");
  if (!in)
    {
      throw ConstruoError ("World: Couldn't open " + filename);
      return;
    }

  lisp_stream_init_file (&stream, in);
  
  lisp_object_t* root_obj = lisp_read (&stream);
  lisp_object_t* cur = lisp_car(root_obj);
  
  if (!lisp_symbol_p (cur))
    {
      throw ConstruoError ("World: Read error in " + filename);
    }
  
  if (strcmp(lisp_symbol(cur), "construo-scene") == 0)
    {
      parse_scene (lisp_cdr(root_obj));
    }
  else
    {
      throw ConstruoError ("World: Read error in " + filename + ". Couldn't find 'construo-scene'");
    }
  
  lisp_free (root_obj);

  std::cout << "particles: " << particles.size () << std::endl;
  std::cout << "springs:   " << sticks.size () << std::endl;
}

void
World::parse_scene (lisp_object_t* cursor)
{
  while(!lisp_nil_p(cursor))
    {
      lisp_object_t* cur = lisp_car(cursor);

      if (!lisp_cons_p(cur) || !lisp_symbol_p (lisp_car(cur)))
        {
          throw ConstruoError ("World: Read error in parse_scene");
        }
      else
        {
          if (strcmp(lisp_symbol(lisp_car(cur)), "particles") == 0)
            {
              parse_particles(lisp_cdr(cur));
            }
          else if (strcmp(lisp_symbol(lisp_car(cur)), "springs") == 0)
            {
              parse_springs(lisp_cdr(cur));
            }
          else
            {
              throw ConstruoError ("World: Read error in parse_scene" 
                                   ". Unhandled " + std::string(lisp_symbol(lisp_car(cur))));
            }
        }      
      cursor = lisp_cdr (cursor);
    }
}

void
World::parse_springs (lisp_object_t* cursor)
{
  while(!lisp_nil_p(cursor))
    {
      lisp_object_t* cur = lisp_car(cursor);
      sticks.push_back(new Stick (this, cur));
      cursor = lisp_cdr (cursor);
    }  
}

void
World::parse_particles (lisp_object_t* cursor)
{
  while(!lisp_nil_p(cursor))
    {
      lisp_object_t* cur = lisp_car(cursor);
      particles.push_back(new Particle (cur));
      cursor = lisp_cdr (cursor);
    }
}

World::World (const World& w)
{
  for (CParticleIter i = w.particles.begin (); 
       i != w.particles.end (); 
       ++i)
    {
      particles.push_back (new Particle (*(*i)));
    }

  for (CStickIter i = w.sticks.begin (); i != w.sticks.end (); ++i)
    {
      Particle* first  = lookup_particle((*i)->particles.first->get_id());
      Particle* second = lookup_particle((*i)->particles.second->get_id());

      if (first && second)
        {
          sticks.push_back (new Stick (first, second, (*i)->length));
        }
      else
        {
          std::cout << "World: Error couldn't resolve particles" << std::endl;
        }
    }
}

World::~World ()
{
  clear ();
}

Particle*
World::lookup_particle (int id)
{
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
World::draw (GraphicContext* gc)
{
  for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
    (*i)->draw ();

  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    (*i)->draw (gc);
}

void
World::update (float delta)
{
  // Main Movement and Forces
  for (int k = 0;  k < 20; ++k)
    {
      {
        // FIXME: Hardcoded Force Emitters
        for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
          {
            // Gravity
            (*i)->add_force (CL_Vector (0.0, 1.0));
		    
            // Central Gravity force:
            /*CL_Vector direction = ((*i)->pos - CL_Vector (400, 300));
              if (direction.norm () != 0.0f)
              (*i)->add_force (direction * (-100.0f/(direction.norm () * direction.norm ())));
            */
            
            /*
              for (ParticleIter j = particles.begin (); j != particles.end (); ++j)
              {
              CL_Vector diff = (*j)->pos - (*i)->pos;
              if (diff.norm () != 0.0f)
              (*i)->add_force (diff * ((10.0f - (*j)->mass)/(diff.norm () * diff.norm ())));
              }	    */
          }

        for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
          (*i)->update (delta);

        for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
          {
            (*i)->update (delta);
          }
      }
    }

  // Stick splitting
  for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
    {
      if ((*i)->destroyed)
        {
          if ((((*i)->particles.first->pos 
                - (*i)->particles.second->pos)).norm () > 10.0f)
            {
              CL_Vector pos = 
                ((*i)->particles.first->pos + (*i)->particles.second->pos)*0.5f;
              Particle* p1 = new Particle (pos, CL_Vector ());
              Particle* p2 = new Particle (pos, CL_Vector ());
              p1->velocity = (*i)->particles.first->velocity * 0.5f;
              p2->velocity = (*i)->particles.second->velocity * 0.5f;
              particles.push_back (p1);
              particles.push_back (p2);
              sticks.push_back (new Stick ((*i)->particles.first, p1));
              sticks.push_back (new Stick ((*i)->particles.second, p2));
            }
        }
    }

  sticks.remove_if (stick_destroyed);
}

Particle* 
World::get_particle (int x, int y)
{
  Particle* particle = 0;
  float min_dist = 15;
  CL_Vector mouse_pos (x, y);

  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      CL_Vector diff = mouse_pos - (*i)->pos;
      if (diff.norm () < min_dist)
	{
	  min_dist = diff.norm ();
	  particle = *i;
	}
    }

  return particle;
}

void
World::add_spring (Particle* last_particle, Particle* particle)
{
  sticks.push_back (new Stick (last_particle, particle));
}

void
World::add_particle (Particle* p)
{
  particles.push_back (p);
}

void
World::remove_particle (Particle* p)
{
  // Remove everyting that references the particle
  for (StickIter i = sticks.begin (); i != sticks.end ();)
    {
      if ((*i)->particles.first == p || (*i)->particles.second == p)
        {
          delete *i;
          i = sticks.erase(i);
        }
      else
        {
          ++i;
        }
    }

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
World::remove_stick (Stick* s)
{
  sticks.remove(s);
}

void
World::clear ()
{
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    delete *i;
 
  for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
    delete *i;

  particles.clear ();
  sticks.clear ();
}

void
World::write_lisp (const std::string& filename)
{
  FILE* out;

  out = fopen(filename.c_str(), "w");

  if (!out)
    {
      std::cout << "World: Couldn't open '" << filename << "' for writing" << std::endl;
      return; 
    }

  std::cout << "Writing to: " << filename << std::endl;

  fputs(";; Written by " PACKAGE_STRING "\n", out);
  fputs("(construo-scene\n", out);
  //fputs("  (version 1)\n", out);
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

  fputs("  (springs\n", out);
  for (CStickIter i = sticks.begin (); i != sticks.end (); ++i)
    {
      lisp_object_t* obj = (*i)->serialize ();
      fputs("    ", out);
      lisp_dump (obj, out);
      lisp_free(obj);
      fputc('\n', out);
    }
  fputs("  )", out);
  fputs(")\n\n;; EOF ;;\n", out);

  fclose(out);
}

/* EOF */
