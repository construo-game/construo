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

#ifndef HEADER_CONSTRUO_PARTICLE_HXX
#define HEADER_CONSTRUO_PARTICLE_HXX

#include <iostream>
#include "construo.hxx"
#include "graphic_context.hxx"
#include "vector.hxx"

class Particle
{
public:
  // FIXME: Place this in a factory
  static unsigned id_counter;

  /** Id of the particle */
  unsigned int id;

  /** position of the particle */
  CL_Vector pos;

  /** velocity of the particle */
  CL_Vector  velocity;

  /** the mass of the particle as 1/mass */
  float mass;

  bool fixed;

  /** totale force acting on particle (used as temp-var in update() to
      collect the forces)*/
  CL_Vector totale_force;

  Particle (CL_Vector arg_pos, CL_Vector arg_velocity)
    : id (++id_counter), pos (arg_pos),
      velocity (arg_velocity)
  {
    fixed = false;
    mass = 10.0;
  }

  void set_id (unsigned int arg_id)
  {
    if (arg_id > id_counter)
      id_counter = arg_id;
    id = arg_id;
  }

  unsigned int get_id ()
  {
    return id;
  }

  void write_xml (std::ostream& out)
  {
    out << "    <particle id=\"" << id << "\">"
      	<< "<position>" << to_xml (pos) << "</position>"
	<< "<velocity>" << to_xml (velocity) << "</velocity>"
	<< "</particle>" << std::endl;
  }

  void add_force (CL_Vector force)
  {
    if (fixed) return;
    totale_force += force;
  }

  void clear_force ()
  {
    totale_force = CL_Vector ();
  }

  void set_fixed (bool f) {
    fixed = f;
  }

  bool get_fixed () {
    return fixed;
  }  

  void update (float delta) 
  {
    if (fixed) return;

    velocity += totale_force * mass * delta;

    pos += velocity * delta;

    float damp = 0.2;

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
    } else if (pos.y > 599) {
      velocity.y =  -fabs(velocity.y);
      pos.y = 599;
      velocity *= damp;
    }

    /*
      CL_Vector dist = pos - CL_Vector (400, 300);
      if (dist.norm () < 50.0f)
      {
      velocity = -velocity;
      }*/
    clear_force ();
  }

  void draw (GraphicContext* gc)
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
  }

  /** draws the particle in highlight mode (aka if mouse is over it) */
  void draw_highlight (GraphicContext* gc)
  {
    //int size = int(10.0f/(mass*mass)) + 1;
    draw (gc);
    graphic_context->draw_fill_circle (int(pos.x), int (pos.y),
                                       6,
                                       Color(1.0f, 1.0f, 1.0f));
  }
};

#endif

/* EOF */
