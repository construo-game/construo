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

#include "math.hpp"
#include "construo_error.hpp"
#include "world.hpp"
#include "particle_factory.hpp"
#include "system_context.hpp"
#include "controller.hpp"
#include "rect.hpp"
#include "rect_collider.hpp"
#include "string_utils.hpp"

World* World::current_world = 0;

World::World ()
  : particle_mgr (new ParticleFactory(this))
{
  file_version = 0;
  has_been_run = false;
}

World::World (const std::string& filename)
  : particle_mgr (0)
{
  std::cout << "World: Loading '" << filename << "'..." << std::endl;
  file_version = 0;

  has_been_run = false;

  ReaderDocument doc = ReaderDocument::from_file(system_context->translate_filename(filename));
  if (doc.get_name() != "construo-scene") {
    throw ConstruoError ("World: Read error in " + filename + ". Couldn't find 'construo-scene'");
  }

  parse_scene(doc.get_mapping());

  ConstruoAssert(particle_mgr, "No Particles given in file, load failed");

  //std::cout << "particles: " << particle_mgr->size () << std::endl;
  //std::cout << "springs:   " << springs.size () << std::endl;
}

// Copy Constructor
World::World (const World& old_world)
{
  file_version = 0;

  for (Colliders::const_iterator i = old_world.colliders.begin();
       i != old_world.colliders.end();
       ++i)
    {
      colliders.push_back((*i)->duplicate());
    }

  // FIXME: Could need optimizations
  particle_mgr = new ParticleFactory (this, *old_world.particle_mgr);

  for (CSpringIter i = old_world.springs.begin (); i != old_world.springs.end (); ++i)
    {
      Particle* first  = particle_mgr->lookup_particle((*i)->particles.first->get_id());
      Particle* second = particle_mgr->lookup_particle((*i)->particles.second->get_id());

      if (first && second)
        {
          // FIXME: Use copy c'tor here maxstiffnes and Co. aren't copied correctly
          springs.push_back(new Spring (first, second, (*i)->length));
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
World::parse_scene(ReaderMapping const& reader)
{
  reader.read("version", file_version);

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
    springs.push_back(new Spring(this, item.get_mapping()));
  }
}

void
World::parse_colliders(ReaderCollection const& collection)
{
  for(auto const& item : collection.get_objects()) {
    if (item.get_name() == "rect") {
      colliders.push_back(new RectCollider(item.get_mapping()));
    } else {
      std::cout << "WARNING: Unknown collider type '" << item.get_name()
                << "' skipping" << std::endl;
    }
  }
}

void
World::parse_particles(ReaderCollection const& collection)
{
  particle_mgr = new ParticleFactory(this, collection);
}

void
World::draw (ZoomGraphicContext* gc)
{
  // FIXME: This is *not* used in the WorldViewComponent!

  current_world = this;

  draw_colliders(gc);
  draw_springs(gc);
  draw_particles(gc);
}

void
World::draw_springs(ZoomGraphicContext* gc)
{
#ifdef NEW_SPRING_CODE
  std::vector<GraphicContext::Line> lines (springs.size());

  Vector2d dist = springs[0]->particles.first->pos - springs[0]->particles.second->pos;
  float stretch = fabs(dist.norm ()/springs[0]->length - 1.0f) * 10.0f;
  float color = fabs((stretch/springs[0]->max_stretch));

  for (unsigned int i = 0; i < springs.size(); ++i)
    {
      //(*i)->draw (gc);
      lines[i].x1 = springs[i]->particles.first->pos.x;
      lines[i].y1 = springs[i]->particles.first->pos.y;
      lines[i].x2 = springs[i]->particles.second->pos.x;
      lines[i].y2 = springs[i]->particles.second->pos.y;
    }
  gc->draw_lines (lines, Color(color, 1.0f - color, 0.0f), 2);
#else
  for (SpringIter i = springs.begin(); i != springs.end(); ++i)
    {
      (*i)->draw (gc);
    }
#endif
}

void
World::draw_particles(ZoomGraphicContext* gc)
{
  particle_mgr->draw(gc);
}

void
World::draw_colliders(ZoomGraphicContext* gc)
{
  for (Colliders::iterator i = colliders.begin (); i != colliders.end (); ++i)
    {
      (*i)->draw(gc);
    }
}

void
World::update (float delta)
{
  current_world = this;

  has_been_run = true;

  // Main Movement and Forces
  // FIXME: Hardcoded Force Emitters
  for (ParticleFactory::ParticleIter i = particle_mgr->begin (); i != particle_mgr->end (); ++i)
    {
      // Gravity
      (*i)->add_force (Vector2d (0.0, 15.0f) * (*i)->get_mass ());
		
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

  //std::cout << "Colliders: " << colliders.size () << std::endl;
  for (Colliders::iterator i = colliders.begin (); i != colliders.end (); ++i)
    (*i)->bounce ();

  // Spring splitting
  std::vector<Spring*> new_springs;
  for (SpringIter i = springs.begin (); i != springs.end (); ++i)
    {
      if ((*i)->destroyed)
        {
          if ((*i)->length > 20.0f)
            {
              // Calc midpoint
              Vector2d pos = ((*i)->particles.first->pos
                               + (*i)->particles.second->pos) * 0.5f;

              // FIXME: particle mass needs to be recalculated
              Particle* p1 = particle_mgr->add_particle (pos, (*i)->particles.first->velocity * 0.5f, .1f);
              Particle* p2 = particle_mgr->add_particle (pos, (*i)->particles.second->velocity * 0.5f, .1f);

              // FIXME: Insert a more sofistikated string splitter here
              new_springs.push_back (new Spring ((*i)->particles.first, p1, (*i)->length/2));
              new_springs.push_back (new Spring ((*i)->particles.second, p2, (*i)->length/2));
            }
        }
    }
  springs.insert(springs.end(), new_springs.begin(), new_springs.end ());

  // Remove any springs that are marked as destroyed
  // FIXME: Could be faster
  SpringIter i = springs.begin ();
  while ( i != springs.end ())
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
World::get_spring (float x, float y)
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
World::get_particle (float x, float y)
{
  Particle* particle = 0;
  float min_dist = 25.0f; // FIXME: Make this configurable
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

std::vector<Particle*>
World::get_particles (float x1_, float y1_, float x2_, float y2_)
{
  float x1 = Math::min(x1_, x2_);
  float x2 = Math::max(x1_, x2_);
  float y1 = Math::min(y1_, y2_);
  float y2 = Math::max(y1_, y2_);

  std::vector<Particle*> caputred_particles;
  for (ParticleFactory::ParticleIter i = particle_mgr->begin (); i != particle_mgr->end (); ++i)
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
  for (ParticleFactory::ParticleIter i = get_particle_mgr()->begin();
       i != get_particle_mgr()->end (); ++i)
    {
      (*i)->velocity = Vector2d ();
    }
}

void
World::add_spring (Particle* last_particle, Particle* particle)
{
  assert (last_particle && particle);
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
  //std::cout << "particles: " << particle_mgr->size () << std::endl;
  //std::cout << "springs:   " << springs.size () << std::endl;

  delete s;
  springs.erase(std::remove(springs.begin (), springs.end (), s),
                springs.end ());
}

void
World::remove_collider (Collider* c)
{
  delete c;
  colliders.erase(std::remove(colliders.begin (), colliders.end (), c),
                  colliders.end ());
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
  std::cout << "World: Writing to: " << filename << std::endl;

  LispWriter writer = LispWriter::from_file(system_context->translate_filename(filename));

  writer.write_comment("Written by " PACKAGE_STRING);
  writer.begin_object("construo-scene");
  writer.write("version",  3);
  writer.write("author", std::vector<std::string>({
        system_context->get_user_realname(),
        system_context->get_user_email()}));

  particle_mgr->write_lisp(writer);

  writer.begin_collection("springs");
  for (CSpringIter i = springs.begin (); i != springs.end (); ++i) {
    (*i)->serialize(writer);
  }
  writer.end_collection();

  writer.begin_collection("colliders");
  for (Colliders::iterator i = colliders.begin(); i != colliders.end(); ++i) {
    (*i)->serialize (writer);
  }
  writer.end_collection();
  writer.end_object();
}

BoundingBox
World::calc_bounding_box()
{
  BoundingBox bbox;

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
      bbox.join((*i)->pos);
    }

  for (Colliders::iterator i = colliders.begin(); i != colliders.end(); ++i)
    {
      bbox.join((*i)->get_bounding_box());
    }

  return bbox;
}

int
World::get_num_particles()
{
  return particle_mgr->size ();
}

int
World::get_num_springs()
{
  return springs.size ();
}

void
World::add_rect_collider(const Vector2d& pos1, const Vector2d& pos2)
{
  Rect<float> rect (pos1.x, pos1.y, pos2.x, pos2.y);

  colliders.push_back(new RectCollider(rect.x1, rect.y1, rect.x2, rect.y2));
}

/* EOF */
