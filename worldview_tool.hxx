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

#ifndef HEADER_CONSTRUO_WORLDVIEW_TOOL_HXX
#define HEADER_CONSTRUO_WORLDVIEW_TOOL_HXX

class ZoomGraphicContext;

/** */
class WorldViewTool
{
public:
  WorldViewTool () {}

  virtual ~WorldViewTool () {}

  /** Let the Tool to some initialisation */
  virtual void activate () {}

  /** Let the Tool to some deinitialisation */
  virtual void deactivate () {}

  virtual void draw_background (ZoomGraphicContext* gc) =0;
  virtual void draw_foreground (ZoomGraphicContext* gc) =0;

  virtual void on_button_press (int button_id, int x, int y) {}

  virtual void on_primary_button_press (int x, int y) {}
  virtual void on_primary_button_release (int x, int y) {}

  virtual void on_secondary_button_press (int x, int y) {}
  virtual void on_secondary_button_release (int x, int y) {}
  
  virtual void on_duplicate_press (int x, int y) {}
  virtual void on_delete_press (int x, int y) {}
  virtual void on_fix_press (int x, int y) {}
  virtual void on_join_press (int x, int y) {}
  
  virtual void on_mouse_move (int x, int y, int of_x, int of_y) {} 
};

#endif

/* EOF */
