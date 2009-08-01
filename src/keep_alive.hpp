//  $Id$
// 
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

#ifndef HEADER_CONSTRUO_KEEP_ALIVE_HPP
#define HEADER_CONSTRUO_KEEP_ALIVE_HPP

#include "keep_alive_mgr.hpp"

/** Each object that inherit from KeepAlive gets the keep_alive()
    function called once per game-loop, it is meant to catch XEvents
    and things like that. */
class KeepAlive
{
private:
public:
  KeepAlive () {
    KeepAliveMgr::register_obj(this);
  }

  virtual ~KeepAlive () {
    KeepAliveMgr::unregister_obj(this);
  }

  virtual void keep_alive () =0;
};

#endif

/* EOF */
