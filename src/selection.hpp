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

#ifndef HEADER_CONSTRUO_SELECTION_HPP
#define HEADER_CONSTRUO_SELECTION_HPP

#include <list>

#include <glm/glm.hpp>
#include <geom/point.hpp>

#include "fwd.hpp"

/** A class to keep track of a group of selected particles */
class Selection
{
public:
  typedef std::list<Particle*> SelectionLst;

public:
  Selection();

  /** @return Center point of the selection, aka the center of its
      bounding box, result is undefined if called on a empty
      selection */
  glm::vec2 get_center();

  /** Select the particles between p1 (upper/left) and p2
      (bottom/right) */
  void select_particles(geom::fpoint const& p1, geom::fpoint const& p2);

  /** scales the selection by the given factor */
  void scale(float factor, glm::vec2 center);

  void flip();

  /** sets the velocity of all particles to the given one */
  void set_velocity(const glm::vec2 vel);

  /** duplicates all selected objects */
  void duplicate();

  /** clears the selection */
  void clear();

  /** @return true if no particle is in the selection */
  bool empty() const;

  /** Check if the world has changed, if so clear selection FIXME: add
      a bit more docu here */
  void validate();

  void rotate(float rot_angle, glm::vec2 rotate_center);

  /** Join particles that are on nearly the same position
   *
   *  @param toleranz minimum distance of to particles below which they
   *  get joined (in world coordinates)
   */
  void join_doubles(float toleranz);

  SelectionLst::size_type size() { return m_selection.size(); }
  SelectionLst::iterator begin() { return m_selection.begin(); };
  SelectionLst::iterator end()   { return m_selection.end(); };

private:
  /** Collection of particles */
  SelectionLst m_selection;

  /** Pointer to the world that contains the particle this selection
      is pointing to. Used to check if the world has changed, so that
      the selection needs to get cleared. */
  World* m_world;

private:
  Selection (const Selection&);
  Selection& operator=(const Selection&);
};

#endif

/* EOF */
