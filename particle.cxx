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
#include "particle.hxx"

Particle::Particle (int i, const Vector2d& arg_pos, const Vector2d& arg_velocity, float m, bool f)
  : id (i), 
    pos (arg_pos),
    velocity (arg_velocity), 
    mass (m), 
    fixed (f),
    totale_force (0,0)
{
}

Particle::Particle (const Particle& p)
  : id (p.id),
    pos (p.pos),
    velocity (p.velocity),
    mass (p.mass),
    fixed (p.fixed),
    totale_force (0,0)
{
}

lisp_object_t* 
Particle::serialize()
{
  LispWriter obj ("particle");
  obj.write_int ("id", id);
  obj.write_vector ("pos", pos);
  obj.write_vector ("velocity", velocity);
  obj.write_boolean ("fixed", fixed);
  obj.write_float ("mass", mass);
  return obj.get_lisp ();
}

void
Particle::draw_highlight (GraphicContext* gc)
{
  //int size = int(10.0f/(mass*mass)) + 1;
  gc->draw_fill_circle (int(pos.x), int (pos.y),
                        6,
                        Color(1.0f, 1.0f, 1.0f));
}

void
Particle::draw (GraphicContext* gc)
{
  //int size = int(10.0f/(mass*mass)) + 1;
  if (fixed)
    {
      gc->draw_fill_circle (int(pos.x), int (pos.y),
                            4,
                            Color(0.6f, 0.6f, 0.6f));
    }
  else
    {
      gc->draw_fill_circle (int(pos.x), int (pos.y),
                            2,
                            Color(1.0f, 0.0f, 0.0f));
    }

  if (0) // draw normals
    gc->draw_line (int (pos.x), int (pos.y),
                   int (pos.x + velocity.x), int (pos.y + velocity.y),
                   Color (0.0f, 0.0f, 1.0f));
}


void
Particle::update (float delta)
{
  if (fixed) return;

  velocity += totale_force * mass * delta;

  //velocity *= .999999f ;

  pos += velocity * delta;

  float damp = 0.2;
#if 0 // FIXME: Replace this with a generic shape collision handling thing
  // Calc collision with screen x border
  if (pos.x < 0) {
    velocity.x =  fabs(velocity.x);
    pos.x = 0;
    velocity *= damp;
  } else if (pos.x > 799) {
    velocity.x =  -fabs(velocity.x);
    pos.x = 799;
    velocity *= damp;
  }

  // Calc collision with screen y border
  if (pos.y < 0) {
    velocity.y =  fabs(velocity.y);
    pos.y = 0;
    velocity *= damp;
  } else
#endif
    if (pos.y > 599) {
      velocity.y =  -fabs(velocity.y);
      pos.y = 599;
      velocity *= damp;
    }

  /*
    Vector2d dist = pos - Vector2d (400, 300);
    if (dist.norm () < 50.0f)
    {
    velocity = -velocity;
    }*/
  clear_force ();
}


/* EOF */
