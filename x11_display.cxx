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
#include "settings.hxx"
#include "construo_main.hxx"

#include "controller.hxx"
#include "gui_manager.hxx"

extern ConstruoMain* construo_main;
Atom wm_delete_window;

static char zoom_tool_cursor[] = {
    /* -------- -------- */  0x00,
    /* -------- -------- */  0x00,
    /* ------xx xx------ */  0x18,                         
    /* ----xxxx xxxx---- */  0x3c,
    /* ----xxxx xxxx---- */  0x3c,
    /* ------xx xx------ */  0x18,
    /* -------- -------- */  0x00,
    /* -------- -------- */  0x00
};

X11Display::X11Display(int w, int h, bool fullscreen_)
  : doublebuffer (settings.doublebuffer),
    width(w), height(h), shift_pressed (false), fullscreen (fullscreen_)
{
  std::cout << "Opening X11 display" << std::endl;
  display = XOpenDisplay(NULL);

  if (!display)
    throw ConstruoError("X11Display: Couldn't conncet to X server");

  int screen = DefaultScreen(display);
  XSetWindowAttributes attributes;

  attributes.background_pixel  = BlackPixel(display, screen);
  attributes.border_pixel      = WhitePixel(display, screen);
  if (fullscreen)
    attributes.override_redirect = True;
  else
    attributes.override_redirect = False;

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
                         CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask,
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

  if (doublebuffer)
    drawable = XCreatePixmap (display, window, width, height, 
                              DefaultDepth(display, screen));  
  else
    drawable = window;

  XMapRaised(display, window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;
  gcv.line_width = 2;
  gc = XCreateGC(display, window, 
                 GCLineWidth | GCForeground | GCBackground,
                 &gcv);

  if (fullscreen)
    set_fullscreen (true);

  if (0) // custom cursor code
    {
      XColor cursor_fg;
      XColor cursor_bg;

      //set colors here

      Pixmap cursor_pm = XCreateBitmapFromData (display, window, zoom_tool_cursor, 8, 8);
      Cursor cursor = XCreatePixmapCursor(display, cursor_pm, None, &cursor_bg, &cursor_fg, 4, 4);
      XDefineCursor (display, window, cursor);
    }
}

X11Display::~X11Display ()
{
  std::cout << "Closing X11 display" << std::endl;
  if (fullscreen)
    restore_mode ();
  
  if (doublebuffer)
    XFreePixmap (display, drawable);
  
  XDestroyWindow (display, window);
  XCloseDisplay(display); 
}

void
X11Display::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawLine (display, drawable, gc, (int) x1, (int) y1, (int) x2, (int) y2);
}

void
X11Display::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(display, gc, color.get_rgb());
  XFillRectangle (display, drawable, gc, 
                  int(x1), int(y1), 
                  int(x2 - x1), int(y2 - y1));
}

void
X11Display::draw_fill_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, color.get_rgb());
  XFillArc(display, drawable, gc, 
           int(x-r), int(y-r),
           int(r*2), int(r*2), 0, 
           360*64);
}

void
X11Display::draw_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, color.get_rgb());
  XDrawArc(display, drawable, gc, int(x-r), int(y-r), int(r*2.0f), int(r*2.0f), 0, 360*64);
}

void
X11Display::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawRectangle (display, drawable, gc, 
                  int(x1), int(y1), 
                  int(x2 - x1), int(y2 - y1));
}

void
X11Display::draw_string(float x, float y, const std::string& str, Color color)
{
  XSetForeground(display, gc, color.get_rgb());
  XDrawString (display, drawable, gc, int(x), int(y), str.c_str (), str.length ());
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
          ev.button.id = BUTTON_TERTIARY;
        else if (event.xbutton.button == 3)
          ev.button.id = BUTTON_SECONDARY;
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
          ev.button.id = BUTTON_TERTIARY;
        else if (event.xbutton.button == 3)
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

          case XK_v:
            send_button_press(BUTTON_SETVELOCITY);
            break;

          case XK_Shift_L:
          case XK_Shift_R:
            shift_pressed = true;
            break;
          case XK_m:
            send_button_press(BUTTON_MODE_CHANGE);
            break;
          case XK_f:
            send_button_press(BUTTON_FIX);
            break;
          case XK_h:
            send_button_press(BUTTON_FLIP);
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
          case XK_d:
            send_button_press(BUTTON_DUPLICATE);
            break;
          case XK_space:
            send_button_press(BUTTON_RUN);
            break;
          case XK_Tab:
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
X11Display::flip (int x1, int y1, int x2, int y2)
{
  if (doublebuffer)
    {
      FlipRect flip_rect;

      flip_rect.x1 = x1;
      flip_rect.y1 = y1;
      flip_rect.x2 = x2;
      flip_rect.y2 = y2;

      //flip_rects.push_back(flip_rect);
    }
}

void
X11Display::real_flip ()
{
  if (doublebuffer)
    {
      for (std::vector<FlipRect>::iterator i = flip_rects.begin ();
           i != flip_rects.end ();
           ++i)
        {
          XCopyArea (display, drawable, window, gc,
                     i->x1, i->y1, // source
                     i->x2 - i->x1, i->y2 - i->y1, // width/height
                     i->x1, i->y1 // destination
                     );
        }
      flip_rects.clear ();
    }
}

void
X11Display::flip ()
{
  if (doublebuffer)
    {
      // FIXME: Use another gc here
      XCopyArea (display, drawable, window, gc,
                 0, 0, // source
                 width, height,
                 0, 0 // destination
                 );
      //XFlush(display);
    }
}

void
X11Display::set_fullscreen (bool fullscreen)
{
#ifdef HAVE_LIBXXF86VM
  int event_base;
  int error_base;
  if (XF86VidModeQueryExtension(display, &event_base, &error_base) != True)
    {
      // No VidMode extension available, bailout
      std::cout << "X11Display: VidMode extension not available, bailout." << std::endl;
      return;
    }

  int screen = DefaultScreen(display);

  memset(&orig_modeline, 0, sizeof(orig_modeline));
  // Get the current display settings for later restore
  XF86VidModeGetModeLine(display, 
                         screen,
                         &orig_dotclock,
                         &orig_modeline);

  XF86VidModeGetViewPort(display,
                         screen,
                         &orig_viewport_x,
                         &orig_viewport_y);

  XF86VidModeModeInfo **modes;
  int nmodes;
  int mode_index = -1;
  if (XF86VidModeGetAllModeLines(display,
                                 screen,
                                 &nmodes,&modes))
    {
      std::cout << "VideoModes: (searching for " << width << "x" << height << ")" << std::endl;
      for (int i = 0; i < nmodes; i++)
        {
          //std::cout << i << "  " << mode.Width,mode.Height);
          std::cout << "  " << modes[i]->hdisplay
                    << "x" << modes[i]->vdisplay;

          if (modes[i]->hdisplay == width && modes[i]->vdisplay == height)
            {
              std::cout << " <-";
              mode_index = i;
            }
          std::cout << std::endl;
        }

      if (mode_index != -1) // Found a good mode
        {
          if (0)
            { // FIXME: doesn't work to change override_redirect after window creation
              std::cout << "Changing override_redirect" << std::endl;
              // Switch border away and go to 0,0
              XSetWindowAttributes attributes;
              attributes.override_redirect = True;
              XChangeWindowAttributes(display, window, CWOverrideRedirect, &attributes);
            }

          std::cout << "Switching to: "
                    << modes[mode_index]->hdisplay << "x" << modes[mode_index]->vdisplay
                    << std::endl;

          if(XF86VidModeSwitchToMode(display,
                                     screen,
                                     modes[mode_index]))
            {
              XF86VidModeSetViewPort(display, screen, 0, 0);
              // Hijack the focus (works only till the next focus change)
              XSetInputFocus(display, window, RevertToParent, CurrentTime);
              
              // Capture the pointer
              if (XGrabPointer(display, window, true, 0, GrabModeAsync, GrabModeAsync, window, None, CurrentTime)
                  != GrabSuccess)
                {
                  std::cout << "X11Display: Couldn't grab the pointer" << std::endl;
                }
            }
        }
      else // No mode found
        {
          std::cout << "Disabling override redirect" << std::endl;
          // Fullscreen not possible, switch Window attributes back to windowed mode
          XSetWindowAttributes attributes;
          attributes.override_redirect = False;
          XChangeWindowAttributes(display, window, CWOverrideRedirect, &attributes);

          // Remap the Window to let the allow override to take effect
          XUnmapWindow (display, window);
          XMapRaised(display, window);
        }
    }
  else
    {
      std::cout << "X11Display: Couldn't get available video modes" << std::endl;
    }
#endif
}

void
X11Display::run()
{
  while (!GUIManager::instance ()->finished ())
    {
      if (Controller::instance()->is_running())
        {
          system_context->sleep (0); // limit CPU usage via brute force
          wait_for_events();
        }
      else
        {
          wait_for_events_blocking();
        }
      GUIManager::instance ()->run_once();
    }
}

void
X11Display::restore_mode ()
{
#ifdef HAVE_LIBXXF86VM
  XF86VidModeModeInfo modeinfo;
  
  modeinfo.dotclock   = orig_dotclock;
  
  // Copy XF86VidModeModeLine struct into XF86VidModeModeInfo
  modeinfo.hdisplay   = orig_modeline.hdisplay;
  modeinfo.hsyncstart = orig_modeline.hsyncstart;
  modeinfo.hsyncend   = orig_modeline.hsyncend;
  modeinfo.htotal     = orig_modeline.htotal;
  modeinfo.hskew      = orig_modeline.hskew;
  modeinfo.vdisplay   = orig_modeline.vdisplay;
  modeinfo.vsyncstart = orig_modeline.vsyncstart;
  modeinfo.vsyncend   = orig_modeline.vsyncend;
  modeinfo.vtotal     = orig_modeline.vtotal;
  modeinfo.flags      = orig_modeline.flags;
  modeinfo.privsize   = orig_modeline.privsize;
  modeinfo.c_private  = orig_modeline.c_private;

  XF86VidModeSwitchToMode(display, DefaultScreen(display),
                          &modeinfo);
  XF86VidModeSetViewPort(display, DefaultScreen(display),
                         orig_viewport_x, orig_viewport_y);

  fullscreen = false;
#endif
}

void
X11Display::set_clip_rect (int x1, int y1, int x2, int y2)
{
  XRectangle rect[1];

  rect[0].x = x1;
  rect[0].y = y1;
  rect[0].width  = x2 - x1;
  rect[0].height = y2 - y1;

  XSetClipRectangles (display, gc, 
                      0, 0, // clip origin
                      rect, 1,
                      Unsorted);
}

/* EOF */
