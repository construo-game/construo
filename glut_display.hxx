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

#ifndef HEADER_CONSTRUO_GLUT_DISPLAY_HXX
#define HEADER_CONSTRUO_GLUT_DISPLAY_HXX

#include "root_graphic_context.hxx"
#include "input_context.hxx"

/** */
class GlutDisplay : public RootGraphicContext,
                    public InputContext
{
private:
  static GlutDisplay* instance_;

  int width;
  int height;

  int mouse_x;
  int mouse_y;

  bool block;

  int update_display;

  bool is_fullscreen;
public:
  static GlutDisplay* instance () { return instance_; }

  GlutDisplay (int w, int h);
  virtual ~GlutDisplay();

  void draw_lines (std::vector<Line>& lines, Color color, int wide = 0);
  void draw_line(float x1, float y1, float x2, float y2, Color color, int wide = 0);
  void draw_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_fill_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_circle(float x, float y, float r, Color color);
  void draw_circles(std::vector<Circle>& circles, Color color);
  void draw_fill_circle(float x, float y, float r, Color color);
  void draw_string(float x, float y, const std::string& str, Color color);
  void draw_string_centered(float x, float y, const std::string& str, Color color);

  void clear ();
  void flip ();
  void flip (int x1, int y1, int x2, int y2);

  void set_fullscreen (bool fullscreen);
  bool get_fullscreen () { return is_fullscreen; }

  int get_width()  { return width; }
  int get_height() { return height; }

  bool get_key (int key);
  int  get_mouse_x ();
  int  get_mouse_y ();

  void run();
  void set_cursor_real(CursorType);
  void reshape_func(int w, int h);
  void display_func ();
  void mouse_func (int button, int button_state, int x, int y);
  void idle_func ();
  void keyboard_func (unsigned char key, int x, int y);
  void mouse_motion_func (int x, int y);

  void set_clip_rect (int x1, int y1, int x2, int y2);
  
  void push_quick_draw();
  void pop_quick_draw();
private:
  GlutDisplay (const GlutDisplay&);
  GlutDisplay& operator= (const GlutDisplay&);
};

#endif

/* EOF */
