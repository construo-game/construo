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

#include "construo_error.hxx"
#include "stick.hxx"

Stick::Stick (World* world, lisp_object_t* cursor)
{
  cursor = lisp_cdr(cursor); // Skip the identifer
  
  int first_id = -1;
  int second_id = -1;
  LispReader reader(cursor);
  reader.read_int ("first", &first_id);
  reader.read_int ("second", &second_id);
  reader.read_float ("length", &length);

  particles.first = world->lookup_particle (first_id);
  particles.second = world->lookup_particle (second_id);

  if (particles.first == 0 || particles.second == 0)
    {
      throw ConstruoError ("Spring: Pair lookup failed");
    }
}

/* EOF */
