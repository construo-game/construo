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

#ifndef CONFIG_HH
#define CONFIG_HH

#include <string>
#include <stdlib.h>

class Config
{
private:
  std::string home_dir;
  std::string construo_dir;

public:
  Config () {
    char* home = getenv ("HOME");
    if (home) {
      home_dir = home;
    } else {
      home_dir = ".";
    }
    construo_dir = home_dir + "/.construo/";
  }
  
  ~Config () {
  }
  
  /** Returns the home directory or the current directory */
  std::string get_home () 
  {
    return home_dir;
  }

  /** Returns the directory to which construo will save its
      quicksave's and manual saves */
  std::string get_construo_dir () {
    return construo_dir;
  }
};

#endif

/* EOF */
