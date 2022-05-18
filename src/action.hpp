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

#ifndef HEADER_CONSTRUO_ACTION_HPP
#define HEADER_CONSTRUO_ACTION_HPP

enum class Action
{
  NONE,

  PRIMARY,   // primary mouse button (inserts particles)
  SECONDARY, // secondary mouse button (delete particles, rotate selection)
  TERTIARY,  // scroll around

  FIX,       // fix a particle at the current position
  ESCAPE,    // escape key
  START,     // space key or third mouse button
  CLEAR,     // clear the current scene
  DELETE,    // delete the current object
  DUPLICATE,    // duplicate the current select
  FLIP,      // flip the current object horizontally
  UNDO,      // bring the world back to its state before the simulation start
  REDO,      // bring the world back to its state before the last undo
  ZEROVELOCITY,   // set velocity to zero
  SETVELOCITY,   // set the velocity of the selection
  SCALE,     // Scale the currently selected object
  TOGGLESLOWMO,
  FULLSCREEN,  // Toggle fullscreen mode
  JOIN,  // Join objects that are close together
  GRID,  // display or hide the grid
  RUN,         // starts or stops the simulation
  MODE_CHANGE, // Switch the input mode (aka what happens when you click with the mouse)
  ACTIONCAM,
  HIDEDOTS,

  ZOOM_OUT,
  ZOOM_IN,

  SCROLL_LEFT,
  SCROLL_RIGHT,
  SCROLL_UP,
  SCROLL_DOWN,

  QUICKLOAD0,
  QUICKLOAD1,
  QUICKLOAD2,
  QUICKLOAD3,
  QUICKLOAD4,
  QUICKLOAD5,
  QUICKLOAD6,
  QUICKLOAD7,
  QUICKLOAD8,
  QUICKLOAD9,

  QUICKSAVE0,
  QUICKSAVE1,
  QUICKSAVE2,
  QUICKSAVE3,
  QUICKSAVE4,
  QUICKSAVE5,
  QUICKSAVE6,
  QUICKSAVE7,
  QUICKSAVE8,
  QUICKSAVE9,
};

#endif

/* EOF */
