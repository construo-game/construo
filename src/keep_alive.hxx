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

#ifndef HEADER_CONSTRUO_KEEP_ALIVE_HXX
#define HEADER_CONSTRUO_KEEP_ALIVE_HXX

#include "keep_alive_mgr.hxx"

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
