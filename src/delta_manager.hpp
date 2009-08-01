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

#ifndef HEADER_CONSTRUO_DELTAMANAGER_HPP
#define HEADER_CONSTRUO_DELTAMANAGER_HPP

#include "construo.hpp"
#include "system_context.hpp"

class DeltaManager
{
private:
  unsigned int last_time;
public:
  DeltaManager ()
    : last_time (system_context->get_time ())
  {}

  float getset ()
  {
    float ret = get ();
    set ();
    return ret;
  }
  
  void set () 
  {
    last_time = system_context->get_time ();
  }
  
  float get () 
  {
    return ( system_context->get_time () - last_time) / 1000.0f;
  }
};

#endif

/* EOF */
