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
#include "config.h"
#include "x11_display.hxx"
#include "construo_main.hxx"

extern ConstruoMain* construo_main;
Atom wm_delete_window;

X11Display::X11Display(int w, int h)
  : width(w), height(h), shift_pressed (false)
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
    KeyReleaseMask       |
    ExposureMask         | 
    PointerMotionMask    |
    ButtonPressMask      |
    ButtonReleaseMask    |
    StructureNotifyMask  |
    ExposureMask;

  window = XCreateWindow(display, RootWindow(display, screen),
                         0,0, // position
                         width, height, 0,
                         CopyFromParent, InputOutput, CopyFromParent, 
                         CWBackPixel|CWBorderPixel|CWEventMask,
                         &attributes);

  { // Communicate a bit with the window manager
    char *title = "Construo " VERSION;

    XTextProperty text_property;
    XStringListToTextProperty(&title, 1, &text_property);
    XSizeHints size_hints;
    size_hints.x = 0;
    size_hints.y = 0;
    size_hints.flags  = PSize | PMinSize | PMaxSize;

    size_hints.width  = width;
    size_hints.height = height;
    
    size_hints.min_width  = width;
    size_hints.min_height = height;
    size_hints.max_width  = width;
    size_hints.max_height = height;

    XSetWMProperties(
                     display,
                     window,
                     &text_property,
                     &text_property,
                     0,
                     0,
                     &size_hints,
                     0,
                     0);

    // Set WM_DELETE_WINDOW atom in WM_PROTOCOLS property (to get window_delete requests).
    wm_delete_window = XInternAtom (display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols (display, window, &wm_delete_window, 1);
  }

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
  XDestroyWindow (display, window);
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
  XSetForeground(display, gc, color.get_rgb());
  XFillRectangle (display, drawable, gc, 
                  x1, y1, 
                  x2 - x1, y2 - y1);
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
  XSetForeground(display, gc, color.get_rgb());
  XDrawRectangle (display, drawable, gc, 
                  x1, y1, 
                  x2 - x1, y2 - y1);
}

void
X11Display::draw_string(int x, int y, const std::string& str, Color color)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawString (display, drawable, gc, x, y, str.c_str (), str.length ());
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
X11Display::get_key (int key)
{
  return false;
}

void
X11Display::wait_for_events_blocking ()
{
  do {
    while (read_event () == false); 
  } while (XPending (display) > 0);
}

void
X11Display::wait_for_events ()
{
  while (XPending (display) > 0)
    {
      read_event ();
    }
}

bool
X11Display::read_event ()
{
  XEvent event;

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

    case NoExpose:
      //std::cout << "NoExpose" << std::endl;
      return false; // FIXME: Hack, no idea how to handle NoExpose
      break;

    case ButtonPress:
      {
        Event ev;
        ev.button.type = BUTTON_EVENT;
        if (event.xbutton.button == 1)
          ev.button.id = BUTTON_PRIMARY;
        else if (event.xbutton.button == 2)
          ev.button.id = BUTTON_SECONDARY;
        else if (event.xbutton.button == 3)
          ev.button.id = BUTTON_DELETE; // FIXME: SECONDARY/Delete mapping should happen elsewhere
        else if (event.xbutton.button == 4)
          ev.button.id = BUTTON_ZOOM_OUT;
        else if (event.xbutton.button == 5)
          ev.button.id = BUTTON_ZOOM_IN;

        ev.button.pressed = true;

        events.push(Event(ev));
      }
      break;

    case ButtonRelease:
      {
        Event ev;
        ev.button.type = BUTTON_EVENT;

        if (event.xbutton.button == 1)
          ev.button.id = BUTTON_PRIMARY;
        else if (event.xbutton.button == 2)
          ev.button.id = BUTTON_SECONDARY;

        ev.button.pressed = false;

        events.push(Event(ev));
      }
      break;

    case KeyPress:
      {
        KeySym sym = XLookupKeysym(&event.xkey,0);
            
        switch (sym)
          {
          case XK_Left:
            send_button_press(BUTTON_SCROLL_LEFT);
            break;

          case XK_Right:
            send_button_press(BUTTON_SCROLL_RIGHT);
            break;

          case XK_Up:
            send_button_press(BUTTON_SCROLL_UP);
            break;

          case XK_Down:
            send_button_press(BUTTON_SCROLL_DOWN);
            break;

          case XK_Shift_L:
          case XK_Shift_R:
            shift_pressed = true;
            break;
          case XK_f:
            send_button_press(BUTTON_FIX);
            break;
          case XK_c:
            send_button_press(BUTTON_CLEAR);
            break;
          case XK_Delete:
            send_button_press(BUTTON_DELETE);
            break;
          case XK_Escape:
            send_button_press(BUTTON_ESCAPE);
            break;
          case XK_u:
            send_button_press(BUTTON_UNDO);
            break;
          case XK_r:
            send_button_press(BUTTON_REDO);
            break;
          case XK_space:
            send_button_press(BUTTON_TOGGLESLOWMO);
            break;

          case XK_0:
            send_load_or_save(0);
            break;
          case XK_1:
            send_load_or_save(1);
            break;
          case XK_2:
            send_load_or_save(2);
            break;
          case XK_3:
            send_load_or_save(3);
            break;
          case XK_4:
            send_load_or_save(4);
            break;
          case XK_5:
            send_load_or_save(5);
            break;
          case XK_6:
            send_load_or_save(6);
            break;
          case XK_7:
            send_load_or_save(7);
            break;
          case XK_8:
            send_load_or_save(8);
            break;
          case XK_9:
            send_load_or_save(9);
            break;
              
          default:
            std::cout << "X11Display: unhandled keypress: " << sym << " " << XK_f << std::endl;
            break;
          }
      }
      break;

    case KeyRelease:
      {
        KeySym sym = XLookupKeysym(&event.xkey,0);
            
        switch (sym)
          {
          case XK_Shift_L:
          case XK_Shift_R:
            shift_pressed = false;
            break;
          default:
            //std::cout << "X11Display: unhandled keyrelease: " << sym << " " << XK_f << std::endl;
            break;
          }
      }
      break;

    case ConfigureNotify:
      //std::cout << "X11Display: " << event.xconfigure.width << "x" << event.xconfigure.height 
      //<< "+" << event.xconfigure.x << "+" << event.xconfigure.y << std::endl;
      break;

    case DestroyNotify:
      std::cout << "Window got destroyed" << std::endl;
      break;

    case ClientMessage:
      std::cout << "X11Display: got client message" << std::endl;
      // Window close request
      if ((int) event.xclient.data.l[0] == (int) wm_delete_window) {
        std::cout << "Window is destroyed" << std::endl;
        send_button_press(BUTTON_ESCAPE);
      }
      break;

    default: 
      std::cout << "X11Display: Unhandled event: " << event.type << std::endl;
      break;
    }
  return true;
}

void
X11Display::keep_alive ()
{
  while (XPending (display) > 0)
    {
      wait_for_events ();
    }
}

void
X11Display::send_load_or_save(int n)
{
  if (shift_pressed)
    send_button_press(BUTTON_QUICKLOAD0 + n);
  else
    send_button_press(BUTTON_QUICKSAVE0 + n);
}

void
X11Display::send_button_press (int i)
{
  Event ev;
  ev.button.type = BUTTON_EVENT;
  ev.button.id = i;
  ev.button.pressed = true;
  events.push(Event(ev)); 
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
