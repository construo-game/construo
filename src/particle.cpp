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

#include <cmath>

#include <glm/gtx/io.hpp>

#include "colors.hpp"
#include "math.hpp"
#include "reader.hpp"
#include "string_utils.hpp"
#include "particle.hpp"

using namespace StringUtils;

Particle::Particle (int i, const glm::vec2& arg_pos, const glm::vec2& arg_velocity, float m, bool f)
  : id (i),
    pos (arg_pos),
    velocity (arg_velocity),
    mass (m),
    fixed (f),
    totale_force (0,0),
    spring_links (0)
{
}

Particle::Particle (const Particle& p)
  : id (p.id),
    pos (p.pos),
    velocity (p.velocity),
    mass (p.mass),
    fixed (p.fixed),
    totale_force (0,0),
    spring_links (0)
{
}

void
Particle::serialize(prio::Writer& writer)
{
  writer.begin_object("particle")
    .write("id", id)
    .write("pos", pos)
    .write("velocity", velocity)
    .write("fixed", fixed)
    .write("mass", mass)
    .end_object();
}

void
Particle::update (float delta)
{
  const float max_velocity = 1000.0f;

  if (fixed) return;

  velocity += totale_force * delta * (1.0f/mass);

  // damping
  if (0)
    velocity -= (velocity * (1.0f/mass) * delta) * 0.001f;

  //velocity *= .999999f ;

  pos += velocity * delta;

  float collision_damp = 0.2f;

#if 0 // FIXME: Replace this with a generic shape collision handling thing
  // Calc collision with screen x border
  if (pos.x < 0) {
    velocity.x =  std::fabs(velocity.x);
    pos.x = 0;
    velocity *= collision_damp;
  } else if (pos.x > 799) {
    velocity.x =  -std::fabs(velocity.x);
    pos.x = 799;
    velocity *= collision_damp;
  }

  // Calc collision with screen y border
  if (pos.y < 0) {
    velocity.y =  std::fabs(velocity.y);
    pos.y = 0;
    velocity *= collision_damp;
  } else
#endif

    if (pos.y > 599) {
      velocity.y =  -std::fabs(velocity.y);
      pos.y = 599;
      velocity *= collision_damp;
    }

  /*
    glm::vec2 dist = pos - glm::vec2 (400, 300);
    if (glm::length(dist) < 50.0f)
    {
    velocity = -velocity;
    }*/
  clear_force ();

  // Avoid to fast things
  if (glm::length(velocity) > max_velocity)
    {
      velocity = glm::normalize(velocity);
      velocity *= max_velocity;
    }
}


/* EOF */
