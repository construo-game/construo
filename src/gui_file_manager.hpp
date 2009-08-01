//  $Id$
// 
//  Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_GUI_FILE_MANAGER_HPP
#define HEADER_CONSTRUO_GUI_FILE_MANAGER_HPP

#include <map>
#include <string>
#include "gui_child_manager.hpp"
#include "gui_directory.hpp"

/** */
class GUIFileManager : public GUIChildManager
{
private:
  /** Cache for directories */
  std::map<std::string, GUIDirectory*> directories;

  GUIDirectory* current_directory;
  
  GUIDirectory* get_directory (const std::string& pathname);

  static GUIFileManager* instance_;
public:
  enum Mode { LOAD_MANAGER, SAVE_MANAGER };
private:
  Mode mode;
public:
  GUIFileManager (Mode m);
  ~GUIFileManager ();

  static GUIFileManager* instance() { return instance_; }
  static void set_instance(GUIFileManager* g) { instance_ = g; }
  
  void draw_overlay (GraphicContext* gc);

  /** Switch the GUIFileManagers view to the directory given by
      pathname */
  void open_directory (const std::string& pathname);

  /** move one directory up, aka cut the last directory from a
      directory pathname */
  void directory_up();

  void update_current_directory();

  void scroll_up ();
  void scroll_down ();
};

#endif

/* EOF */
