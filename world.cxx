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

#include <algorithm>
#include "config.h"
#include "math.hxx"
#include "construo_error.hxx"
#include "world.hxx"
#include "particle_factory.hxx"
#include "system_context.hxx"

World* World::current_world = 0; 

World::World ()
  : particle_mgr (new ParticleFactory())
{
  has_been_run = false;
}

World::World (const std::string& filename)
  : particle_mgr (0)
{
  has_been_run = false;
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

  ConstruoAssert(particle_mgr, "No Particles given in file, load failed");

  std::cout << "particles: " << particle_mgr->size () << std::endl;
  std::cout << "springs:   " << springs.size () << std::endl;
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
              std::cout << "World: Read error in parse_scene. Unhandled tag '" 
                        << lisp_symbol(lisp_car(cur)) << "' skipping and continuing" << std::endl;
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
      springs.push_back(new Spring (this, cur));
      cursor = lisp_cdr (cursor);
    }  
}

void
World::parse_particles (lisp_object_t* cursor)
{
  particle_mgr = new ParticleFactory(cursor);
}

World::World (const World& old_world)
{
  particle_mgr = new ParticleFactory (*old_world.particle_mgr);
  
  for (CSpringIter i = old_world.springs.begin (); i != old_world.springs.end (); ++i)
    {
      Particle* first  = particle_mgr->lookup_particle((*i)->particles.first->get_id());
      Particle* second = particle_mgr->lookup_particle((*i)->particles.second->get_id());

      if (first && second)
        {
          springs.push_back (new Spring (first, second, (*i)->length));
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

void
World::draw (GraphicContext* gc)
{
  current_world = this;

  for (SpringIter i = springs.begin (); i != springs.end (); ++i)
    (*i)->draw (gc);

  particle_mgr->draw(gc);

  //const WorldBoundingBox& box = calc_bounding_box();
  //gc->draw_rect((int) box.x1, (int) box.y1, (int) box.x2, (int) box.y2, Color (0x0000FF));
}

void
World::update (float delta)
{
  current_world = this;

  has_been_run = true;

  // Main Movement and Forces
  for (int k = 0;  k < 20; ++k)
    {
      {
        // FIXME: Hardcoded Force Emitters
        for (ParticleFactory::ParticleIter i = particle_mgr->begin (); i != particle_mgr->end (); ++i)
          {
            // Gravity
            (*i)->add_force (Vector2d (0.0, 1.0));
		    
            // Central Gravity force:
            /*Vector2d direction = ((*i)->pos - Vector2d (400, 300));
              if (direction.norm () != 0.0f)
              (*i)->add_force (direction * (-100.0f/(direction.norm () * direction.norm ())));
            */
            
            /*
              for (ParticleIter j = particles.begin (); j != particles.end (); ++j)
              {
              Vector2d diff = (*j)->pos - (*i)->pos;
              if (diff.norm () != 0.0f)
              (*i)->add_force (diff * ((10.0f - (*j)->mass)/(diff.norm () * diff.norm ())));
              }	    */
          }

        for (SpringIter i = springs.begin (); i != springs.end (); ++i)
          (*i)->update (delta);

        particle_mgr->update(delta);
      }
    }

  // Spring splitting
  std::vector<Spring*> new_springs;
  for (SpringIter i = springs.begin (); i != springs.end (); ++i)
    {
      if ((*i)->destroyed)
        {
          if ((((*i)->particles.first->pos 
                - (*i)->particles.second->pos)).norm () > 10.0f)
            {
              // Calc midpoint
              Vector2d pos = ((*i)->particles.first->pos
                               + (*i)->particles.second->pos) * 0.5f;

              Particle* p1 = particle_mgr->add_particle (pos, (*i)->particles.first->velocity * 0.5f);
              Particle* p2 = particle_mgr->add_particle (pos, (*i)->particles.second->velocity * 0.5f);

              new_springs.push_back (new Spring ((*i)->particles.first, p1));
              new_springs.push_back (new Spring ((*i)->particles.second, p2));
            }
        }
    }
  springs.insert(springs.end(), new_springs.begin(), new_springs.end ());

  // Remove any springs that are marked as destroyed
  // FIXME: Could be faster
  for (SpringIter i = springs.begin (); i != springs.end ();)
    {
      if ((*i)->destroyed)
        {
          delete *i;
          i = springs.erase(i);
        }
      else
        {
          ++i;
        }
    }
}

Spring*
World::get_spring (int x, int y)
{
  Spring* spring = 0;
  float min_distance = 0.0f;

  float capture_threshold = 15;

  for (SpringIter i = springs.begin (); i != springs.end (); ++i)
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
      
      float distance = (fabs((x2 - x1)*(y1-y0) - (x1-x0)*(y2-y1))
                        / sqrt((x2-x1)*(x2-x1) + (y2-y1)*(y2-y1)));
      
      if (u >= 0 && u <= 1.0f
          && ((spring && min_distance > distance)
              || (!spring && distance <= capture_threshold))) // FIXME: threashold is dependend on view
        {
          spring = *i;
          min_distance = distance;
        }
    }

  return spring;
}

Particle* 
World::get_particle (int x, int y)
{
  Particle* particle = 0;
  float min_dist = 15;
  Vector2d mouse_pos (x, y);

  for (ParticleFactory::ParticleIter i = particle_mgr->begin (); i != particle_mgr->end (); ++i)
    {
      Vector2d diff = mouse_pos - (*i)->pos;
      if (diff.norm () < min_dist)
	{
	  min_dist = diff.norm ();
	  particle = *i;
	}
    }

  return particle;
}

void 
World::zero_out_velocity ()
{
  std::cout << "Setting velocity to zero" << std::endl;
  for (ParticleFactory::ParticleIter i = get_particle_mgr()->begin(); 
       i != get_particle_mgr()->end (); ++i)
    {
      (*i)->velocity = Vector2d ();
    }
}

void
World::add_spring (Particle* last_particle, Particle* particle)
{
  springs.push_back (new Spring (last_particle, particle));
}

void
World::remove_particle (Particle* p)
{
  // Remove everyting that references the particle
  for (SpringIter i = springs.begin (); i != springs.end ();)
    {
      if ((*i)->particles.first == p || (*i)->particles.second == p)
        {
          delete *i;
          // FIXME: this is potentially slow, since we don't care
          // about order, we could speed this up
          i = springs.erase(i);
        }
      else
        {
          ++i;
        }
    }

  particle_mgr->remove_particle(p);
}

void
World::remove_spring (Spring* s)
{
  std::cout << "particles: " << particle_mgr->size () << std::endl;
  std::cout << "springs:   " << springs.size () << std::endl;

  delete s;
  springs.erase(std::remove(springs.begin (), springs.end (), s), 
                springs.end ());
}

void
World::clear ()
{
  particle_mgr->clear();
 
  for (SpringIter i = springs.begin (); i != springs.end (); ++i)
    delete *i;

  springs.clear ();
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
  fputs("  (version 2)\n", out);
  // FIXME: insert creation date here
  // FIXME: Filter '()"' here
  fprintf(out, "  (author \"%s\" \"%s\")\n", 
          system_context->get_user_realname().c_str(),
          system_context->get_user_email().c_str());
  particle_mgr->write_lisp(out);
  fputs("  (springs\n", out);
  for (CSpringIter i = springs.begin (); i != springs.end (); ++i)
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

WorldBoundingBox
World::calc_bounding_box()
{
  WorldBoundingBox bbox;

  if (particle_mgr->size() > 0)
    {
      bbox.x1 = bbox.x2 = (*particle_mgr->begin ())->pos.x;
      bbox.y1 = bbox.y2 = (*particle_mgr->begin ())->pos.y;
    }
  else
    {
      bbox.x1 = 0;
      bbox.y1 = 0;

      bbox.x2 = 800;
      bbox.y2 = 600;
    }

  for (ParticleFactory::ParticleIter i = particle_mgr->begin (); i != particle_mgr->end (); ++i)
    {
      bbox.x1 = Math::min(bbox.x1, (*i)->pos.x);
      bbox.y1 = Math::min(bbox.y1, (*i)->pos.y);

      bbox.x2 = Math::max(bbox.x2, (*i)->pos.x);
      bbox.y2 = Math::max(bbox.y2, (*i)->pos.y);
    }

  return bbox;
}

/* EOF */
