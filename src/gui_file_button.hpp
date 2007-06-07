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

#ifndef HEADER_CONSTRUO_GUI_FILE_BUTTON_HPP
#define HEADER_CONSTRUO_GUI_FILE_BUTTON_HPP

#include <string>
#include "zoom_graphic_context.hpp"
#include "gui_component.hpp"

/** */
class GUIFileButton : public GUIComponent
{
protected:
  std::string filename;
  bool mouse_over;
public:
  GUIFileButton (const std::string& arg_filename);
  ~GUIFileButton ();

  //virtual void draw (ZoomGraphicContext*) =0;
  virtual void on_click() =0;

  void on_mouse_enter () { mouse_over = true; }
  void on_mouse_leave () { mouse_over = false; }

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);
};

#endif

/* EOF */
