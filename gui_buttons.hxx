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

#ifndef HEADER_CONSTRUO_GUI_BUTTONS_HXX
#define HEADER_CONSTRUO_GUI_BUTTONS_HXX

#include "gui_component.hxx"

/** */
class GUIButton : public GUIComponent
{
protected:
  std::string title;
  int x_pos;
  int y_pos;
  int width;
  int height;

  bool mouse_over;
  bool pressed;

public:
  GUIButton (const std::string& title, int x_pos_, int y_pos_, int width_, int height_);

  bool is_at (int x, int y);

  void draw_border_hover(GraphicContext*);
  void draw_border_pressed(GraphicContext*);
  void draw_border_normal(GraphicContext*);
  
  void on_mouse_enter ();
  void on_mouse_leave ();

  void draw (GraphicContext*);

  virtual void draw_content (GraphicContext*);
};

class GUIRunButton : public GUIButton
{
public:
  GUIRunButton ();
  void draw_content (GraphicContext*);
  void on_primary_button_click (int x, int y);
};

class GUISlowMoButton : public GUIButton
{
public:
  GUISlowMoButton ();
  void draw_content (GraphicContext*);
  void on_primary_button_click (int x, int y);
};

class GUIUndoButton : public GUIButton
{
public:
  GUIUndoButton ();
  void draw_content (GraphicContext*);
  void on_primary_button_click (int x, int y);
};

class GUIZoomInButton : public GUIButton
{
public:
  GUIZoomInButton ();
  void on_primary_button_click (int x, int y);
};

class GUIZoomOutButton : public GUIButton
{
public:
  GUIZoomOutButton ();
  void on_primary_button_click (int x, int y);
};


class GUIQuitButton : public GUIButton
{
public:
  GUIQuitButton ();
  void on_primary_button_click (int x, int y);
};

#endif

/* EOF */
