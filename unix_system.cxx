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

#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include "construo_error.hxx"
#include "unix_system.hxx"

UnixSystem::UnixSystem ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);
  start_time = (long) tv.tv_sec*(long) 1000+(long) tv.tv_usec/(long) 1000;

  char* home = getenv("HOME");
  if (home)
    {
      construo_rc_path = std::string(home) + std::string("/.construo/");
    }
  else
    {
      throw ConstruoError ("UnixSystem: Couldn't find $HOME!");
    }

  // create $HOME directory if not already there
  struct stat buf;

  if (stat(construo_rc_path.c_str(), &buf) != 0) // Couldn't find directory, create it
    {
      if (mkdir(construo_rc_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP) != 0)
        {
          throw ConstruoError(std::string("UnixSystem: ") + construo_rc_path + ": "
                              + strerror(errno));
        }
    }
  else
    {
      if (S_ISDIR(buf.st_rdev)) // Is not a directory
        {
          throw ConstruoError("Error: " + construo_rc_path + " is not a directory!");
        }
      
      if (access(construo_rc_path.c_str (), R_OK | W_OK | X_OK) != 0) // not readable/writeable
        {
          throw ConstruoError("Error: " + construo_rc_path + " is not read or writeable!");
        }
    }     
}

UnixSystem::~UnixSystem ()
{
}

unsigned int 
UnixSystem::get_time ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);

  long tid = (long) tv.tv_sec*(long) 1000 + (long) tv.tv_usec/(long) 1000 - start_time;

  return tid;
}

void
UnixSystem::sleep (unsigned long t)
{
  usleep (t);
}

std::string 
UnixSystem::get_construo_rc_path()
{
  return construo_rc_path;
}

std::string
UnixSystem::get_user_realname()
{
  struct passwd* pw;

  pw = getpwuid(getuid());
  if (pw)
    {
      return pw->pw_gecos;
    }
  else
    {
      return "";
    }
}

std::string 
UnixSystem::get_user_email()
{
  const char* s_email = getenv("EMAIL");
  if (s_email)
    {
      return s_email;
    }
  else
    return "";
}

/* EOF */
