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

#ifndef HEADER_CONSTRUO_SELECTION_HXX
#define HEADER_CONSTRUO_SELECTION_HXX

#include <vector>
#include "vector2d.hxx"

class Particle;
class World;

/** A class to keep track of a group of selected particles */
class Selection
{
private:
  typedef std::vector<Particle*> SelectionLst;
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
  void scale (float factor);

  void flip();

  /** duplicates all selected objects */
  void duplicate ();

  /** clears the selection */
  void clear ();
  
  /** @return true if no particle is in the selection */
  bool empty() const;

  void rotate(float rot_angle, Vector2d rotate_center);

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
