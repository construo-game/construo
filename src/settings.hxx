//  $Id$
// 
//  Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_SETTINGS_HXX
#define HEADER_CONSTRUO_SETTINGS_HXX

#include <string>

/** */
class Settings
{
public:
  /** Start the application in fullscreen mode */
  bool fullscreen;

  /** Use doublebuffering */
  bool doublebuffer;

  /** Enable alpha blending */
  bool alphablending;

  /** Enable anti aliasing*/
  bool antialiasing;

  /** Enable thick lines (more than 1 pixel width */
  bool thick_lines;
  
  /** Screen width in pixel */
  int  screen_width;

  /** Screen height in pixel */
  int  screen_height;

  /** Initial file to be loaded on startup (aka. the one that is given
      on the command line */
  std::string startup_file;

  /** directory with data files */
  std::string datadir;

  Settings ();
};

extern Settings settings;

#endif

/* EOF */
