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

#ifndef HEADER_CONSTRUO_INPUT_CONTEXT_HPP
#define HEADER_CONSTRUO_INPUT_CONTEXT_HPP

#include <queue>

#include <geom/point.hpp>

#include "keys.hpp"
#include "action.hpp"
#include "events.hpp"

class InputContext
{
public:
  InputContext () : events() {}
  virtual ~InputContext () {}

  // Polling functions

  virtual geom::fpoint get_mouse_pos() const = 0;

  /** If an event is in the queue, copy it to \a event and return
      true, else return false and leave \a event untouched */
  bool get_event(Event* event);

protected:
  std::queue<Event> events;
};

#endif

/* EOF */
