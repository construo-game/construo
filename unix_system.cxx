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

#include <sys/time.h>
#include <time.h>
#include "unix_system.hxx"

UnixSystem::UnixSystem ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);
  start_time = (long) tv.tv_sec*(long) 1000+(long) tv.tv_usec/(long) 1000;
}

unsigned int 
UnixSystem::get_time ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);

  long tid = (long) tv.tv_sec*(long) 1000 + (long) tv.tv_usec/(long) 1000 - start_time;

  return tid;
}

/* EOF */
