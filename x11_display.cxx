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

#include "construo_error.hxx"
#include "x11_display.hxx"

X11Display::X11Display(int w, int h)
  : width(w), height(h)
{
  display = XOpenDisplay(NULL);

  if (!display)
    throw ConstruoError("X11Display: Couldn't conncet to X server");

  int screen = DefaultScreen(gd_display);
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

  window = XCreateWindow(gd_display, RootWindow(gd_display, gd_screen),
                         0,0, // position
                         width, height, 0,
                         CopyFromParent, InputOutput, CopyFromParent, 
                         CWBackPixel|CWBorderPixel|CWEventMask,
                         &attributes);
  
  XMapRaised(display, window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;
  gc = XCreateGC(display, drawable, GCForeground, &gcv);
}

void
X11Display::draw_line(int x1, int y1, int x2, int y2, int color, int wide)
{
  XSetForceground(display, gc, color);
  XDrawLine (display, window, gc, x1, y1, x2, y2);
}

void
X11Display::draw_filled_rect(int x1, int y1, int x2, int y2, int color)
{
  // not impl
}

/* EOF */
