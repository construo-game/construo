// Construo - A wire-frame construction gamee
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

#include "rect_collider.hpp"

#include <cmath>

#include "colors.hpp"
#include "particle_factory.hpp"
#include "controller.hpp"
#include "construo_error.hpp"

std::unique_ptr<Collider>
RectCollider::duplicate() const
{
  return std::make_unique<RectCollider>(x1, y1, x2, y2);
}

RectCollider::RectCollider (float x1_, float y1_, float x2_, float y2_) :
  x1(std::min(x1_, x2_)),
  y1(std::min(y1_, y2_)),
  x2(std::max(x1_, x2_)),
  y2(std::max(y1_, y2_))
{
}

bool
RectCollider::is_at (const glm::vec2& pos)
{
  return (x1 <= pos.x && x2 > pos.x
          && y1 <= pos.y && y2 > pos.y);
}

glm::vec2
RectCollider::get_pos()
{
  return glm::vec2 ((x1 + x2)/2.0f,
                   (y1 + y2)/2.0f);
}

void
RectCollider::set_pos(const glm::vec2& pos)
{
  glm::vec2 center = get_pos();
  x1 = x1 - center.x + pos.x;
  x2 = x2 - center.x + pos.x;
  y1 = y1 - center.y + pos.y;
  y2 = y2 - center.y + pos.y;
}

void
RectCollider::bounce()
{
  float const damp = 0.8f;
  for (auto& particle : Controller::instance()->get_world().particles())
    {
      glm::vec2& pos = particle->pos;
      glm::vec2& velocity = particle->velocity;

      if (pos.x > x1 && pos.x < x2
          && pos.y > y1 && pos.y < y2)
        {
          float left_dist  = pos.x - x1;
          float right_dist = x2 - pos.x;

          float top_dist    = pos.y - y1;
          float bottom_dist = y2 - pos.y;

          if (left_dist < right_dist
              && left_dist < top_dist
              && left_dist < bottom_dist)
            {
              velocity.x = -std::fabs(velocity.x);
              pos.x = x1;
            }
          else if (right_dist < left_dist
                   && right_dist < top_dist
                   && right_dist < bottom_dist)
            {
              velocity.x = std::fabs(velocity.x);
              pos.x = x2;
            }
          else if (top_dist < left_dist
                   && top_dist < right_dist
                   && top_dist < bottom_dist)
            {
              velocity.y = -std::fabs(velocity.y);
              pos.y = y1;
            }
          else
            {
              velocity.y = std::fabs(velocity.y);
              pos.y = y2;
            }
          velocity *= damp;
        }
    }
}

BoundingBox
RectCollider::get_bounding_box() const
{
  return BoundingBox(x1, y1, x2, y2);
}

/* EOF */
