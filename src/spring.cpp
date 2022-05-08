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

#include "colors.hpp"
#include "construo_error.hpp"
#include "particle_factory.hpp"
#include "spring.hpp"

Spring::Spring(Particle* f, Particle* s, float l) :
  particles(f, s),
  length(l),
  destroyed(false),
  stiffness(50.0f),
  damping(0.1f),
  max_stretch(0.15f)
{
  f->spring_links += 1;
  s->spring_links += 1;
}

Spring::Spring(Particle* f, Particle* s) :
  particles(f, s),
  length(fabsf(glm::length(f->pos - s->pos))),
  destroyed(false),
  stiffness(50.0f),
  damping(0.1f),
  max_stretch(0.15f)
{
  f->spring_links += 1;
  s->spring_links += 1;

  assert (length != 0);
}

Spring::Spring(World* world, ReaderMapping const& reader) :
  particles(nullptr, nullptr),
  length(-1),
  destroyed(false),
  stiffness(50.0f),
  damping(0.1f),
  max_stretch(0.15f)
{
  int first_id = -1;
  int second_id = -1;
  length = -1;

  reader.read("first", first_id);
  reader.read("second", second_id);
  reader.read("length", length);
  reader.read("stiffness", stiffness);
  reader.read("damping", damping);
  reader.read("maxstretch", max_stretch);

  particles.first  = world->get_particle_mgr().lookup_particle(first_id);
  particles.second = world->get_particle_mgr().lookup_particle(second_id);

  if (particles.first == nullptr || particles.second == nullptr)
    {
      throw ConstruoError ("Spring: Pair lookup failed");
    }

  particles.first->spring_links  += 1;
  particles.second->spring_links += 1;

  if (length == -1)
    {
      //std::cout << "Spring: length missing in data file, recalculating" << std::endl;
      length = fabsf(glm::length(particles.first->pos - particles.second->pos));
    }
}

Spring::~Spring ()
{
  particles.first->spring_links -= 1;
  particles.second->spring_links -= 1;
}

void
Spring::update (float delta)
{
  glm::vec2 dist = particles.first->pos - particles.second->pos;

  // Calculate the stretchness of the spring, 0.0 if unstretch, else
  // <> 0
  float stretch = glm::length(dist) - length;

  //std::cout << "Stretch: " << stretch << std::endl;
  if (fabsf(stretch/length) > max_stretch &&
      length > 10.0f) // atomar spring
    { // If the spring is streched above limits, let it get destroyed
      destroyed = true;
    }
  else
    {
      stretch *= stiffness;
      float dterm = (glm::dot(dist, particles.first->velocity - particles.second->velocity) * damping) / glm::length(dist);

      dist = glm::normalize(dist);
      glm::vec2 force = dist * (stretch + dterm);

      /*std::cout << "DTerm: " << dterm << " HTerm: " << stretch
                << " Force: " << force
                << std::endl;*/

      particles.first->add_force (-force);
      particles.second->add_force (force);
    }
}

void
Spring::draw (ZoomGraphicContext& gc)
{
  glm::vec2 dist = particles.first->pos - particles.second->pos;
  float stretch = fabsf(glm::length(dist)/length - 1.0f) * 10.0f;

  float color = fabsf((stretch/max_stretch));

  if (particles.first->pos.y  < 598.5f
      ||
      particles.second->pos.y < 598.5f)
    {
      gc.GraphicContext::draw_line(particles.first->pos,
                                    particles.second->pos,
                                    Color(color, 1.0f - color, 0.0f),
                                    2);
    }
}

void
Spring::draw_highlight (ZoomGraphicContext& gc)
{
  gc.GraphicContext::draw_line (particles.first->pos, particles.second->pos,
                                 g_style.highlight, 4);
}

void
Spring::serialize(prio::Writer& writer)
{
  writer.begin_object("spring")
    .write("first", particles.first->get_id())
    .write("second", particles.second->get_id())
    .write ("length", length)
    .write("stiffness", stiffness)
    .write("damping", damping)
    .write("maxstretch", max_stretch)
    .end_object();
}

void
Spring::recalc_length ()
{
  length = fabsf(glm::length(particles.first->pos - particles.second->pos));
}

/* EOF */
