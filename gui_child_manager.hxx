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

#ifndef HEADER_CONSTRUO_GUI_CHILDMANAGER_HXX
#define HEADER_CONSTRUO_GUI_CHILDMANAGER_HXX

#include <vector>
#include <string>
#include "zoom_graphic_context.hxx"
#include "gui_component.hxx"

/** */
class GUIChildManager : public GUIComponent
{
private:
  ZoomGraphicContext gc;
  typedef std::vector<GUIComponent*> ComponentLst;
  ComponentLst components;

  GUIComponent* current_component;
  GUIComponent* find_component_at (int, int);
public:
  GUIChildManager (int x, int y, int width, int height);
  ~GUIChildManager ();

  void add (GUIComponent*);
  void remove (GUIComponent*);
  void replace(GUIComponent* old_comp, GUIComponent* new_comp);

  void draw (GraphicContext* gc);
  virtual void draw_overlay (GraphicContext* gc) {}

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_click (int x, int y);
  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);
  
  void on_delete_press (int x, int y);
  void on_fix_press (int x, int y);

  virtual void on_mouse_enter ();
  virtual void on_mouse_leave ();

  void wheel_up (int x, int y);
  void wheel_down (int x, int y);

  void scroll_left ();
  void scroll_right ();
  void scroll_up ();
  void scroll_down ();

  void on_mouse_move (int x, int y, int of_x, int of_y);
};

#endif

/* EOF */
