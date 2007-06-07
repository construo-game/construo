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

#ifndef HEADER_CONSTRUO_CONSTRUO_MAIN_HPP
#define HEADER_CONSTRUO_CONSTRUO_MAIN_HPP

#include <vector>
#include <iostream>
#include <list>
#include "config.hpp"

class GUIManager;
class World;
class X11Display;
class GlutDisplay;
class UnixSystem;

class ConstruoMain
{
private:
#ifdef USE_X11_DISPLAY
  X11Display*  display;
#elif USE_GLUT_DISPLAY
  GlutDisplay* display;
#endif
  UnixSystem* system;

  bool do_quit;
  Config config;
  GUIManager* gui_manager;
public:
  ConstruoMain ();
  virtual ~ConstruoMain ();

  char* get_title ();
  int main (int argc, char* argv[]);

  /** Exit ConstruoMain and do all stuff necesarry for a clean
      shutdown */
  void exit();
private:
  /** Called once the game is going to end, used to do the lastsave
      and similar things */
  void on_exit();

  /** Initialize the graphic context and the system */
  void init_system();

  /** Shutdown the graphic display and everything else */
  void deinit_system();

  void process_events ();
};

extern ConstruoMain* construo_main;

#endif

/* EOF */
