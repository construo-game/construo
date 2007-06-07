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

#ifndef HEADER_CONSTRUO_SELECTION_HPP
#define HEADER_CONSTRUO_SELECTION_HPP

#include <list>
#include "vector2d.hpp"

class Particle;
class World;

/** A class to keep track of a group of selected particles */
class Selection
{
private:
  typedef std::list<Particle*> SelectionLst;
  /** Collection of particles */
  SelectionLst selection;

  /** Pointer to the world that contains the particle this selection
      is pointing to. Used to check if the world has changed, so that
      the selection needs to get cleared. */
  World* world;

public:
  Selection ();

  /** @return Center point of the selection, aka the center of its
      bounding box, result is undefined if called on a empty
      selection */
  Vector2d get_center ();

  /** Select the particles between p1 (upper/left) and p2
      (bottom/right) */
  void select_particles (Vector2d p1, Vector2d p2);

  /** scales the selection by the given factor */
  void scale (float factor, Vector2d center);

  void flip();

  /** sets the velocity of all particles to the given one */
  void set_velocity (const Vector2d vel);

  /** duplicates all selected objects */
  void duplicate ();

  /** clears the selection */
  void clear ();
  
  /** @return true if no particle is in the selection */
  bool empty() const;

  /** Check if the world has changed, if so clear selection FIXME: add
      a bit more docu here */
  void validate();

  void rotate(float rot_angle, Vector2d rotate_center);

  /** Join particles that are on nearly the same position 
   *
   *  @param toleranz minimum distance of to particles below which they
   *  get joined (in world coordinates) 
   */
  void join_doubles(float toleranz);

  SelectionLst::size_type size() { return selection.size(); }
  SelectionLst::iterator begin() { return selection.begin(); };
  SelectionLst::iterator end()   { return selection.end(); };

  typedef SelectionLst::iterator iterator;
private:
  Selection (const Selection&);
  Selection& operator= (const Selection&);
};

#endif

/* EOF */
