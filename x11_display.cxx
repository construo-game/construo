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
#include <X11/Xutil.h>
#include <X11/keysym.h>
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

  drawable = XCreatePixmap (display, window, width, height, 
                            DefaultDepth(display, screen));  

  XMapRaised(display, window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;
  gcv.line_width = 2;
  gc = XCreateGC(display, window, 
                 GCLineWidth | GCForeground | GCBackground,
                 &gcv);
}

X11Display::~X11Display ()
{
  std::cout << "Closing X11 display" << std::endl;
  XFreePixmap (display, drawable);
  XCloseDisplay(display);
}

void
X11Display::draw_line(int x1, int y1, int x2, int y2, Color color, int wide)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawLine (display, drawable, gc, x1, y1, x2, y2);
}

void
X11Display::draw_fill_rect(int x1, int y1, int x2, int y2, Color color)
{
  // not impl
}

void
X11Display::draw_fill_circle(int x, int y, int r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, color.get_rgb());
  XFillArc(display, drawable, gc, x-r, y-r, r*2, r*2, 0, 360*64);
}

void
X11Display::draw_circle(int x, int y, int r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, color.get_rgb());
  XDrawArc(display, drawable, gc, x-r, y-r, r*2, r*2, 0, 360*64);
}

void
X11Display::draw_rect(int x1, int y1, int x2, int y2, Color color)
{
}

void
X11Display::draw_string(int x, int y, const std::string& str, Color color)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawImageString (display, drawable, gc, x, y, str.c_str (), str.length ());
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

        case Expose:
          if (event.xexpose.count == 0)
            flip ();
          break;

        case ButtonPress:
          {
            Event ev;
            ev.button.type = BUTTON_EVENT;
            if (event.xbutton.button == 1)
              ev.button.id = BUTTON_PRIMARY;
            else if (event.xbutton.button == 3)
              ev.button.id = BUTTON_DELETE; // FIXME: SECONDARY/Delete mapping should happen elsewhere
            else
              ev.button.id = BUTTON_START;

            ev.button.pressed = true;

            events.push(Event(ev));
          }
          break;

        case KeyPress:
          {
            KeySym sym = XLookupKeysym(&event.xkey,0);
            std::cout << "keypress: " << sym << " " << XK_f << std::endl;
            if (sym == XK_f) // FIXME: a bit much duplicate code
              {
                Event ev;
                ev.button.type = BUTTON_EVENT;
                ev.button.id = BUTTON_FIX;
                ev.button.pressed = true;
                events.push(Event(ev));
              }
            else if (sym == XK_c)
              {
                Event ev;
                ev.button.type = BUTTON_EVENT;
                ev.button.id = BUTTON_CLEAR;
                ev.button.pressed = true;
                events.push(Event(ev));
              }
            else if (sym == XK_Delete)
              {
                Event ev;
                ev.button.type = BUTTON_EVENT;
                ev.button.id = BUTTON_DELETE;
                ev.button.pressed = true;
                events.push(Event(ev));
              }
            else if (sym == XK_Escape)
              {
                Event ev;
                ev.button.type = BUTTON_EVENT;
                ev.button.id = BUTTON_ESCAPE;
                ev.button.pressed = true;
                events.push(Event(ev));
              }
          }
          break;

        default: 
          //std::cout << "X11Display: Unhandled event: " << event.type << std::endl;
          break;
        }
    }
}

void
X11Display::clear ()
{
  XSetForeground (display, gc, 0x000000);
  XFillRectangle (display, drawable, gc, 0, 0, width, height);
}

void
X11Display::flip ()
{
  // FIXME: Use another gc here
  XCopyArea (display, drawable, window, gc,
             0, 0, // source
             width, height,
             0, 0 // destination
             );
  //XFlush(display);
}

/* EOF */
