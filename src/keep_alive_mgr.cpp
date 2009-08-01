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

#include "keep_alive.hpp"
#include "keep_alive_mgr.hpp"

std::vector<KeepAlive*> KeepAliveMgr::lst;

void
KeepAliveMgr::register_obj (KeepAlive* obj)
{
  lst.push_back (obj);
}

void
KeepAliveMgr::unregister_obj (KeepAlive*)
{
  // FIXME:
}

void
KeepAliveMgr::keep_alive ()
{
  for (std::vector<KeepAlive*>::iterator i = lst.begin ();
       i != lst.end (); ++i)
    {
      (*i)->keep_alive ();
    }
}

/* EOF */
