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

#include "colors.hxx"
#include "gui_component.hxx"

/** */
class GUIButton : public GUIComponent
{
protected:
  std::string title;

  bool mouse_over;
  bool pressed;

public:
  GUIButton (const std::string& title, int x_pos_, int y_pos_, int width_, int height_);

  void draw_border_hover(GraphicContext*);
  void draw_border_pressed(GraphicContext*);
  void draw_border_normal(GraphicContext*);
  
  void on_mouse_enter ();
  void on_mouse_leave ();

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void draw (GraphicContext*);

  virtual void draw_content (GraphicContext*);
  virtual void on_click ();
};

class GUIRunButton : public GUIButton
{
public:
  GUIRunButton ();
  void draw_content (GraphicContext*);
  void on_click();
};

class GUISlowMoButton : public GUIButton
{
public:
  GUISlowMoButton ();
  void draw_content (GraphicContext*);
  void on_click();
};

class GUIZoomInButton : public GUIButton
{
public:
  GUIZoomInButton ();
  void on_click();
};

class GUIZoomOutButton : public GUIButton
{
public:
  GUIZoomOutButton ();
  void on_click();
};


class GUIQuitButton : public GUIButton
{
public:
  GUIQuitButton ();
  void on_click();
};

class GUILoadButton : public GUIButton
{
public:
  GUILoadButton ();
  void on_click();
};

inline bool always_false()
{
  return false;
}

class GUIGenericButton : public GUIButton
{
private:
  typedef void (*Func)();
  typedef bool (*HighlightFunc)();
  Func func;
  HighlightFunc hfunc;
public:
  GUIGenericButton (const std::string& title, int x, int y, int width, int height, 
                    Func f, HighlightFunc h = always_false)
    : GUIButton (title, x, y, width, height),
      func (f),
      hfunc(h)
  {
  }

  void on_click () 
  {
    func ();
  }

  void draw_content (GraphicContext* gc)
  {
    if (hfunc())
      gc->draw_fill_rect (x_pos, y_pos,
                          x_pos + width, y_pos + height, Colors::button_bg_active);

    GUIButton::draw_content (gc);
  }
};

#endif

/* EOF */
