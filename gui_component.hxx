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

#ifndef HEADER_CONSTRUO_GUI_COMPONENT_HXX
#define HEADER_CONSTRUO_GUI_COMPONENT_HXX

class GraphicContext;
class GUIManager;

/** A thing that is under the controll of the GUIManager */
class GUIComponent
{
private:
  GUIManager* parent;
  int x;
  int y;
  int width;
  int height;
  
public:
  GUIComponent() {}

  GUIComponent (GUIManager* p, int x_, int y_, int width_, int height_)
    : parent (p), x (x_), y (y_), width (width_), height (height_)
  {}

  virtual ~GUIComponent () {}

  virtual void draw (GraphicContext* gc) =0;

  /** @return true if the component is present at the given location */
  virtual bool is_at (int x, int y) { return false; }

  virtual void on_primary_button_click (int x, int y) {}
  virtual void on_primary_button_press (int x, int y) {}
  virtual void on_primary_button_release (int x, int y) {}

  virtual void on_secondary_button_click (int x, int y) {}
  virtual void on_secondary_button_press (int x, int y) {}
  virtual void on_secondary_button_release (int x, int y) {}
  
  virtual void on_delete_press (int x, int y) {}
  virtual void on_fix_press (int x, int y) {}

  virtual void on_mouse_move (int x, int y, int of_x, int of_y) {}
};

#endif

/* EOF */
