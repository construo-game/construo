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

#include <iostream>
#include "construo_error.hxx"
#include "x11_display.hxx"

X11Display::X11Display(int w, int h)
  : width(w), height(h)
{
  std::cout << "Opening X11 display" << std::endl;
  display = XOpenDisplay(NULL);

  if (!display)
    throw ConstruoError("X11Display: Couldn't conncet to X server");

  int screen = DefaultScreen(display);
  XSetWindowAttributes attributes;

  attributes.background_pixel = BlackPixel(display, screen);
  attributes.border_pixel     = WhitePixel(display, screen);

  attributes.event_mask = 
    KeyPressMask         |
    ExposureMask         | 
    PointerMotionMask    |
    ButtonPressMask      |
    ButtonReleaseMask    |
    StructureNotifyMask;

  window = XCreateWindow(display, RootWindow(display, screen),
                         0,0, // position
                         width, height, 0,
                         CopyFromParent, InputOutput, CopyFromParent, 
                         CWBackPixel|CWBorderPixel|CWEventMask,
                         &attributes);
  
  XMapRaised(display, window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;
  gc = XCreateGC(display, window, GCForeground, &gcv);
}

X11Display::~X11Display ()
{
  std::cout << "Closing X11 display" << std::endl;
  XCloseDisplay(display);
}

void
X11Display::draw_line(int x1, int y1, int x2, int y2, Color color, int wide)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawLine (display, window, gc, x1, y1, x2, y2);
}

void
X11Display::draw_fill_rect(int x1, int y1, int x2, int y2, Color color)
{
  // not impl
}

void
X11Display::draw_rect(int x1, int y1, int x2, int y2, Color color)
{
}

void
X11Display::draw_string(int x, int y, const std::string& str)
{
  
}

int
X11Display::get_mouse_x ()
{
  return mouse_x;
}

int
X11Display::get_mouse_y ()
{
  return mouse_y;
}

bool
X11Display::get_keycode (int key)
{
  return false;
}

void
X11Display::keep_alive ()
{
  //std::cout << "Keep alive" << std::endl;
  XEvent event;

  while (XPending (display) > 0)
    {
      XNextEvent (display, &event);
     
      switch (event.type)
        {
        case MotionNotify:
          mouse_x = event.xmotion.x;
          mouse_y = event.xmotion.y;
          break;

        default: 
          std::cout << "X11Display: Unhandled event: " << event.type << std::endl;
          break;
        }
    }
}

/* EOF */
