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

#include "spring.hpp"

#include <cassert>

#include "colors.hpp"
#include "construo_error.hpp"
#include "particle_factory.hpp"

namespace construo {

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
  length(std::fabs(glm::length(f->pos - s->pos))),
  destroyed(false),
  stiffness(50.0f),
  damping(0.1f),
  max_stretch(0.15f)
{
  f->spring_links += 1;
  s->spring_links += 1;

  assert (length != 0);
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

  if (std::fabs(stretch/length) > max_stretch &&
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

      particles.first->add_force (-force);
      particles.second->add_force (force);
    }
}

void
Spring::recalc_length ()
{
  length = std::fabs(glm::length(particles.first->pos - particles.second->pos));
}

} // namespace construo

/* EOF */
