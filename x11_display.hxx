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
#include <X11/Xutil.h>

#ifdef HAVE_LIBXXF86VM
#  include <X11/extensions/xf86vmode.h>
#endif

#include "math.hxx"
#include "graphic_context.hxx"
#include "input_context.hxx"

#define X11_FULLSCREEN_MODE true
#define X11_WINDOW_MODE     false

struct FlipRect
{
  int x1;
  int y1;
  int x2;
  int y2;
};

/** X11Display driver */
class X11Display : public GraphicContext,
                   public InputContext
{
private:
  bool doublebuffer;
#ifdef HAVE_LIBXXF86VM
  XF86VidModeModeLine orig_modeline;
#endif
  int orig_viewport_x;
  int orig_viewport_y;
  int orig_dotclock;

  int       width;
  int       height;
  Display*  display;
  Window    window;
  Colormap  colormap;
  Drawable  drawable;
  GC        gc;

  bool shift_pressed;
  int  mouse_x;
  int  mouse_y;

  /** true if display is in fullscreen mode, false for window mode */
  bool fullscreen;

  std::vector<FlipRect> flip_rects;
  std::vector<FlipRect> last_flip_rects;

  void set_fullscreen();
  void save_mode();
  void restore_mode();
public:
  X11Display (int w, int h, bool fullscreen_);
  virtual ~X11Display ();

  // Graphic Context stuff
  void draw_lines (std::vector<Line>& lines, Color color, int wide = 0);
  void draw_line(float x1, float y1, float x2, float y2, Color color, int wide = 0);
  void draw_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_fill_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_circle(float x, float y, float r, Color color);
  void draw_circles(std::vector<Circle>& circles, Color color);

  void draw_fill_circle(float x, float y, float r, Color color);
  void draw_string(float x, float y, const std::string& str, Color color);
  void draw_string_centered(float x, float y, const std::string& str, Color color);

  int get_width () { return width; }
  int get_height () { return height; }

  void toggle_fullscreen();

  void clear ();
  
  /** Flip the double buffered display */
  void flip ();

  /** perform the real flip, only flip marked reagions */
  void real_flip ();

  void flip (int x1, int y1, int x2, int y2);

  // Input Context stuff
  int get_mouse_x ();
  int get_mouse_y ();

  bool get_key (int key);

  /** Waits for events to come in, blocks until new events are available */
  void wait_for_events_blocking ();

  void wait_for_events ();

  void run();

  void set_clip_rect (int x1_, int y1_, int x2_, int y2_);

  unsigned int get_color_value(Color& color);
private:
  bool read_event ();
  void send_button_press (int i);
  void send_button_release (int i);
  void send_load_or_save(int n);

  X11Display (const X11Display&);
  X11Display& operator= (const X11Display&);
};

#endif

/* EOF */
