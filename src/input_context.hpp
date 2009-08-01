//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_CONSTRUO_INPUT_CONTEXT_HPP
#define HEADER_CONSTRUO_INPUT_CONTEXT_HPP

#include <queue>
#include "vector2d.hpp"
#include "keys.hpp"
#include "buttons.hpp"
#include "events.hpp"

/** */
class InputContext
{
protected:
  std::queue<Event> events;
public:
  InputContext () {}
  virtual ~InputContext () {}

  // Polling functions

  /** @return true if the button with the given keycode is currently
      pressed, false otherwise */
  virtual bool get_key (int key) =0;
  
  /** @return mouse x coordinate */
  virtual int  get_mouse_x () =0;
  
  /** @return mouse y coordinate */
  virtual int  get_mouse_y () =0;

  Vector2d get_mouse_pos () { return Vector2d(get_mouse_x (), get_mouse_y()); }

  /** If an event is in the queue, copy it to \a event and return
      true, else return false and leave \a event untouched */
  bool get_event(Event* event);
};

#endif

/* EOF */
