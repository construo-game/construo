//  $Id$
//
//  Construo - A wire-frame construction gamee
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
#include "colors.hxx"
#include "particle_factory.hxx"
#include "controller.hxx"
#include "rect_collider.hxx"
#include "construo_error.hxx"

Collider*
RectCollider::duplicate() const
{
  return new RectCollider(x1, y1, x2, y2);
}

RectCollider::RectCollider (lisp_object_t* cursor)
{
  Vector2d pos1, pos2;
  
  LispReader reader(cursor);
  if (reader.read_vector("pos1", &pos1) == false
      || reader.read_vector("pos2", &pos2) == false)
    {
      throw ConstruoError("RectCollider entry incomplete");
    }

  x1 = pos1.x;
  y1 = pos1.y;
  x2 = pos2.x;
  y2 = pos2.y;
}

RectCollider::RectCollider (float x1_, float y1_, float x2_, float y2_)
  : x1 (x1_), y1 (y1_), x2 (x2_), y2 (y2_)
{
}

bool
RectCollider::is_at (const Vector2d& pos)
{
  return (x1 <= pos.x && x2 > pos.x
          && y1 <= pos.y && y2 > pos.y);
}

Vector2d
RectCollider::get_pos()
{
  return Vector2d ((x1 + x2)/2.0f,
                   (y1 + y2)/2.0f); 
}

void
RectCollider::set_pos(const Vector2d& pos)
{
  Vector2d center = get_pos();
  x1 = x1 - center.x + pos.x;
  x2 = x2 - center.x + pos.x;
  y1 = y1 - center.y + pos.y;
  y2 = y2 - center.y + pos.y;
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
  gc->draw_fill_rect (x1, y1, x2, y2, Colors::rect_collider_bg);
  gc->draw_rect (x1, y1, x2, y2, Colors::rect_collider_fg);
}

void
RectCollider::draw_highlight (GraphicContext* gc)
{
  //gc->draw_fill_rect (x1, y1, x2, y2, Colors::rect_collider_bg);
  gc->draw_rect (x1, y1, x2, y2, Colors::selection_rect);
}

lisp_object_t*
RectCollider::serialize()
{
  LispWriter obj ("rect");
  obj.write_vector ("pos1", Vector2d(x1, y1));
  obj.write_vector ("pos2", Vector2d(x2, y2));
  return obj.create_lisp ();
}

/* EOF */
