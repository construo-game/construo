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

#ifndef HEADER_CONSTRUO_SCREEN_MANAGER_HXX
#define HEADER_CONSTRUO_SCREEN_MANAGER_HXX

class GUIManager;

/** */
class ScreenManager
{
private:
  bool do_quit;

  GUIManager* current_gui_manager;

  GUIManager* load_gui_manager;
  GUIManager* world_gui_manager;
  
  static ScreenManager* instance_;
  ScreenManager ();

public:
  enum { LOAD_GUI, WORLD_GUI };
  static ScreenManager* instance ();
  
  void set_gui (int gui_id);

  void quit();

  bool is_finished ();

  void run_once ();
};

#endif

/* EOF */
