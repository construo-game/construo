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

#ifndef HEADER_X11_DISPLAY_HXX
#define HEADER_X11_DISPLAY_HXX

#include <X11/Xlib.h>
#include <X11/extensions/xf86vmode.h>

#include "graphic_context.hxx"
#include "input_context.hxx"
#include "keep_alive.hxx"

/** X11Display driver */
class X11Display : public GraphicContext,
                   public InputContext,
                   public KeepAlive
{
private:
  XF86VidModeModeLine orig_modeline;
  int orig_viewport_x;
  int orig_viewport_y;
  int orig_dotclock;

  int       width;
  int       height;
  Display*  display;
  Window    window;
  Drawable  drawable;
  GC        gc;

  bool shift_pressed;
  int  mouse_x;
  int  mouse_y;
public:
  X11Display (int w, int h);
  virtual ~X11Display ();

  // Graphic Context stuff
  void draw_line(int x1, int y1, int x2, int y2, Color color, int wide = 0);
  void draw_rect(int x1, int y1, int x2, int y2, Color color);
  void draw_fill_rect(int x1, int y1, int x2, int y2, Color color);
  void draw_circle(int x, int y, int r, Color color);
  void draw_fill_circle(int x, int y, int r, Color color);
  void draw_string(int x, int y, const std::string& str, Color color);

  void set_fullscreen (bool fullscreen);
  void restore_mode ();

  int get_width () { return width; }
  int get_height () { return height; }

  void clear ();
  
  /** Flip the double buffered display */
  void flip ();

  // Input Context stuff
  int get_mouse_x ();
  int get_mouse_y ();

  bool get_key (int key);

  /** Pools for events (FIXME: should be unused) */
  void keep_alive ();

  /** Waits for events to come in, blocks until new events are available */
  void wait_for_events_blocking ();

  void wait_for_events ();

private:
  bool read_event ();
  void send_button_press (int i);
  void send_load_or_save(int n);

  X11Display (const X11Display&);
  X11Display& operator= (const X11Display&);
};

#endif

/* EOF */
