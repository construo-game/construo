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

#ifndef HEADER_CONSTRUO_BUTTONS_HXX
#define HEADER_CONSTRUO_BUTTONS_HXX

enum {
  BUTTON_PRIMARY,   // primary mouse button (inserts particles)
  BUTTON_SECONDARY, // secondary mouse button (unused)
  BUTTON_FIX,       // fix a particle at the current position
  BUTTON_ESCAPE,    // escape key
  BUTTON_START,     // space key or third mouse button
  BUTTON_CLEAR,     // clear the current scene
  BUTTON_DELETE,    // delete the current object
  BUTTON_UNDO,      // bring the world back to its state before the simulation start
  BUTTON_REDO,      // bring the world back to its state before the last undo
  BUTTON_ZEROVELOCITY,   // set velocity to zero
  BUTTON_TOGGLESLOWMO, 

  BUTTON_ZOOM_OUT,
  BUTTON_ZOOM_IN,

  BUTTON_SCROLL_LEFT,
  BUTTON_SCROLL_RIGHT,
  BUTTON_SCROLL_UP,
  BUTTON_SCROLL_DOWN,

  BUTTON_QUICKLOAD0,
  BUTTON_QUICKLOAD1,
  BUTTON_QUICKLOAD2,
  BUTTON_QUICKLOAD3,
  BUTTON_QUICKLOAD4,
  BUTTON_QUICKLOAD5,
  BUTTON_QUICKLOAD6,
  BUTTON_QUICKLOAD7,
  BUTTON_QUICKLOAD8,
  BUTTON_QUICKLOAD9,

  BUTTON_QUICKSAVE0,
  BUTTON_QUICKSAVE1,
  BUTTON_QUICKSAVE2,
  BUTTON_QUICKSAVE3,
  BUTTON_QUICKSAVE4,
  BUTTON_QUICKSAVE5,
  BUTTON_QUICKSAVE6,
  BUTTON_QUICKSAVE7,
  BUTTON_QUICKSAVE8,
  BUTTON_QUICKSAVE9,
};

#endif

/* EOF */
