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

#ifndef HEADER_CONSTRUO_GUI_DIRECTORY_HXX
#define HEADER_CONSTRUO_GUI_DIRECTORY_HXX

#include <vector>
#include "gui_child_manager.hxx"

class ZoomGraphicContext;
class GUIFileButton;

/** Widget which shows a complete directory */
class GUIDirectory : public GUIChildManager
{
private:
  std::string pathname;
  std::vector<GUIFileButton*> files;
  int offset;
  unsigned int mtime;

  void place_components ();
public:
  enum Mode { LOAD_DIRECTORY, SAVE_DIRECTORY };
private:
  Mode mode;
public:
  GUIDirectory (const std::string& pathname, Mode m);
  ~GUIDirectory ();

  void draw_overlay (GraphicContext* gc);

  std::string get_path() { return pathname; }

  /** Move the shown directory content up */
  void move_up ();
  /** Move the shown directory content down */
  void move_down ();

  void wheel_up (int x, int y);
  void wheel_down (int x, int y);
};

#endif

/* EOF */
