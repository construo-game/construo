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

#include <math.h>
#include "particle_factory.hxx"
#include "controller.hxx"
#include "rect_collider.hxx"

RectCollider::RectCollider (int x1_, int y1_, int x2_, int y2_)
  : x1 (x1_), y1 (y1_), x2 (x2_), y2 (y2_)
{
}

bool
RectCollider::is_at (const Vector2d& pos)
{
  return false;
}

void
RectCollider::bounce ()
{
  ParticleFactory* particle_mgr = Controller::instance()->get_world()->get_particle_mgr();

  float damp = 0.8;
  for (ParticleFactory::ParticleIter i = particle_mgr->begin(); i != particle_mgr->end (); ++i)
    {
      Vector2d& pos = (*i)->pos;
      Vector2d& velocity = (*i)->velocity;

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
              velocity.x = -fabs(velocity.x);
              pos.x = x1;
            }
          else if (right_dist < left_dist
                   && right_dist < top_dist 
                   && right_dist < bottom_dist)
            {
              velocity.x = fabs(velocity.x);
              pos.x = x2;
            }
          else if (top_dist < left_dist
                   && top_dist < right_dist
                   && top_dist < bottom_dist)
            {
              velocity.y = -fabs(velocity.y);
              pos.y = y1;
            }
          else
            {
              velocity.y = fabs(velocity.y);
              pos.y = y2;
            }
          velocity *= damp;
        }
    }
}

void
RectCollider::draw (GraphicContext* gc)
{
  //std::cout << "Drawing collider" << std::endl;
  gc->draw_rect (x1, y1, x2, y2, Color (0x0000FF));
}

/* EOF */
