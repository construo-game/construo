//  $Id$
// 
//  Pingus - A free Lemmings clone
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

#ifndef HEADER_CONSTRUO_GUI_MANAGER_HXX
#define HEADER_CONSTRUO_GUI_MANAGER_HXX

#include <vector>

class GUIComponent;

/** The GUIManager is basically the place where the main loop runs */
class GUIManager
{
private:
  bool do_quit;
  
  /** A collection of GUI components aka widgets */
  typedef std::vector<GUIComponent*> ComponentLst;
  ComponentLst components;

  void process_events ();
  void draw_status ();
  GUIComponent* find_component_at (int, int);
public:
  GUIManager ();
  
  /** Launches the main-loop of the GUIManager, doesn't return until
      quit() is called. */
  void run ();

  /** Stops the GUIManager */
  void quit();
};

#endif

/* EOF */
