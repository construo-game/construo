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
#include <X11/cursorfont.h>
#include <X11/keysym.h>

#include "cursor_insert.xbm"
#include "cursor_insert_mask.xbm"

#include "cursor_select.xbm"
#include "cursor_select_mask.xbm"

#include "cursor_zoom.xbm"
#include "cursor_zoom_mask.xbm"

#include "cursor_scroll.xbm"
#include "cursor_scroll_mask.xbm"

#include "cursor_collider.xbm"
#include "cursor_collider_mask.xbm"

#include "construo_error.hxx"
#include "config.h"
#include "x11_display.hxx"
#include "settings.hxx"
#include "construo_main.hxx"

#include "controller.hxx"
#include "screen_manager.hxx"

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
  /* -------- -------- */  0x00,
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

  colormap = DefaultColormap (display, screen);
  attributes.colormap = colormap;
  window = XCreateWindow(display, RootWindow(display, screen),
                         0,0, // position
                         width, height, 0,
                         CopyFromParent, InputOutput, CopyFromParent, 
                         CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask | CWColormap,
                         &attributes);

  { // Communicate a bit with the window manager
    char *title = construo_main->get_title();

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

  if (settings.thick_lines)
    gcv.line_width = 2;
  else
    gcv.line_width = 0;

  gc = XCreateGC(display, window, 
                 GCLineWidth | GCForeground | GCBackground,
                 &gcv);

  if (fullscreen)
    set_fullscreen();

  {
    // Black&White
    XColor cursor_fg = get_xcolor(Color(1.0f, 1.0f, 1.0f));
    XColor cursor_bg = get_xcolor(Color(0, 0, 0));
      
    cursor_scroll_pix = XCreateBitmapFromData (display, window, (char*)cursor_scroll_bits, 
                                           cursor_scroll_width, cursor_scroll_height);
    cursor_scroll_mask = XCreateBitmapFromData (display, window, (char*)cursor_scroll_mask_bits, 
                                                cursor_scroll_width, cursor_scroll_height);
    cursor_scroll = XCreatePixmapCursor(display, cursor_scroll_pix, cursor_scroll_mask, &cursor_bg, &cursor_fg, 
                                        cursor_scroll_x_hot, cursor_scroll_y_hot);

    cursor_zoom_pix =  XCreateBitmapFromData (display, window, (char*)cursor_zoom_bits, 
                                              cursor_zoom_width, cursor_zoom_height);
    cursor_zoom_mask = XCreateBitmapFromData (display, window, (char*)cursor_zoom_mask_bits, 
                                              cursor_zoom_width, cursor_zoom_height);
    cursor_zoom = XCreatePixmapCursor(display, cursor_zoom_pix, cursor_zoom_mask, &cursor_bg, &cursor_fg, 
                                      cursor_zoom_x_hot, cursor_zoom_y_hot);

    cursor_insert_pix =  XCreateBitmapFromData (display, window, (char*)cursor_insert_bits, 
                                                cursor_insert_width, cursor_insert_height);
    cursor_insert_mask = XCreateBitmapFromData (display, window, (char*)cursor_insert_mask_bits, 
                                                cursor_insert_width, cursor_insert_height);
    cursor_insert = XCreatePixmapCursor(display, cursor_insert_pix, cursor_insert_mask, &cursor_bg, &cursor_fg, 
                                        cursor_insert_x_hot, cursor_insert_y_hot);

    cursor_select_pix =  XCreateBitmapFromData (display, window, (char*)cursor_select_bits, 
                                                cursor_select_width, cursor_select_height);
    cursor_select_mask = XCreateBitmapFromData (display, window, (char*)cursor_select_mask_bits, 
                                                cursor_select_width, cursor_select_height);
    cursor_select = XCreatePixmapCursor(display, cursor_select_pix, cursor_select_mask, &cursor_bg, &cursor_fg, 
                                        cursor_select_x_hot, cursor_select_y_hot);

    cursor_collider_pix =  XCreateBitmapFromData (display, window, (char*)cursor_collider_bits, 
                                                  cursor_collider_width, cursor_collider_height);
    cursor_collider_mask = XCreateBitmapFromData (display, window, (char*)cursor_collider_mask_bits, 
                                                  cursor_collider_width, cursor_collider_height);
    cursor_collider = XCreatePixmapCursor(display, cursor_collider_pix, cursor_collider_mask, &cursor_bg, &cursor_fg, 
                                          cursor_collider_x_hot, cursor_collider_y_hot);
  }

  set_cursor(CURSOR_INSERT);
}

X11Display::~X11Display ()
{
  std::cout << "Closing X11 display" << std::endl;
  
  if (fullscreen)
    {
      std::cout << "X11Display: Restoring video mode" << std::endl;
      restore_mode ();
    }
  
  if (doublebuffer)
    XFreePixmap (display, drawable);
  
  XDestroyWindow (display, window);
  XCloseDisplay(display); 
}

void
X11Display::set_cursor_real(CursorType cursor)
{
  switch(cursor)
    {
    case CURSOR_INSERT:
      XDefineCursor (display, window, cursor_insert);
      break;
    case CURSOR_SCROLL:
      XDefineCursor (display, window, cursor_scroll);
      break;
    case CURSOR_ZOOM:
      XDefineCursor (display, window, cursor_zoom);
      break;
    case CURSOR_COLLIDER:
      XDefineCursor (display, window, cursor_collider);
      break;
    case CURSOR_SELECT:
      XDefineCursor (display, window, cursor_select);
      break;
    default:
      std::cout << "X11Display: Unhandled cursor type: " << cursor << std::endl;
      break;
    }
}

void
X11Display::draw_lines (std::vector<Line>& lines, Color color, int wide)
{
  std::vector<XSegment> segments (lines.size());
  
  for (std::vector<Line>::size_type i = 0; i < lines.size(); ++i)
    {
      segments[i].x1 = static_cast<short>(lines[i].x1);
      segments[i].y1 = static_cast<short>(lines[i].y1);
      segments[i].x2 = static_cast<short>(lines[i].x2);      
      segments[i].y2 = static_cast<short>(lines[i].y2);
    }

  XDrawSegments(display, drawable, gc, &*segments.begin(), segments.size());
}

void
X11Display::draw_circles(std::vector<Circle>& circles, Color color)
{
  std::vector<XArc> arcs (circles.size());
  for (std::vector<Circle>::size_type i = 0; i < circles.size(); ++i)
    {
      arcs[i].x = static_cast<short>(circles[i].x - circles[i].r);
      arcs[i].y = static_cast<short>(circles[i].y - circles[i].r);
      arcs[i].width  = static_cast<short>(2 * circles[i].r);
      arcs[i].height = static_cast<short>(2 * circles[i].r);
      arcs[i].angle1 = 0;
      arcs[i].angle2 = 360 * 64;
    }

  XSetForeground(display, gc, get_color_value(color));
  XFillArcs(display, drawable, gc, 
            &*arcs.begin(), arcs.size());
}

void
X11Display::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  XSetForeground(display, gc, get_color_value(color));
  XDrawLine (display, drawable, gc, (int) x1, (int) y1, (int) x2, (int) y2);
}

void
X11Display::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(display, gc, get_color_value(color));
  XFillRectangle (display, drawable, gc, 
                  int(x1), int(y1), 
                  int(x2 - x1), int(y2 - y1));
}

void
X11Display::draw_fill_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, get_color_value(color));
  XFillArc(display, drawable, gc, 
           int(x-r), int(y-r),
           int(r*2), int(r*2), 0, 
           360*64);
}

void
X11Display::draw_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(display, gc, get_color_value(color));
  XDrawArc(display, drawable, gc, int(x-r), int(y-r), int(r*2.0f), int(r*2.0f), 0, 360*64);
}

void
X11Display::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(display, gc, get_color_value(color));
  XDrawRectangle (display, drawable, gc, 
                  int(x1), int(y1), 
                  int(x2 - x1), int(y2 - y1));
}

void
X11Display::draw_string(float x, float y, const std::string& str, Color color)
{
  XSetForeground(display, gc, get_color_value(color));
  XDrawString (display, drawable, gc, int(x), int(y), str.c_str (), str.length ());
}

void
X11Display::draw_string_centered(float x, float y, const std::string& str, Color color)
{
  XSetForeground(display, gc, get_color_value(color));
  XDrawString (display, drawable, gc, 
               int(x) - ((str.length() * 6) / 2), int(y), 
               str.c_str (), str.length ());
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
        //std::cout << "ButtonID: " << event.xbutton.button << " " << event.xbutton.state << std::endl;

        if (event.xbutton.button == 1)
          send_button_press(BUTTON_PRIMARY);
        else if (event.xbutton.button == 2)
          send_button_press(BUTTON_TERTIARY);
        else if (event.xbutton.button == 3)
          send_button_press(BUTTON_SECONDARY);
        else if (event.xbutton.button == 4)
          send_button_press(BUTTON_ZOOM_IN);
        else if (event.xbutton.button == 5)
          send_button_press(BUTTON_ZOOM_OUT);
      }
      break;

    case ButtonRelease:
      {
        //std::cout << "ButtonID: " << event.xbutton.button << " " << event.xbutton.state << std::endl;
        if (event.xbutton.button == 1)
          send_button_release(BUTTON_PRIMARY);
        else if (event.xbutton.button == 2)
          send_button_release(BUTTON_TERTIARY);
        else if (event.xbutton.button == 3)
          send_button_release(BUTTON_SECONDARY);
        else if (event.xbutton.button == 4)
          send_button_release(BUTTON_ZOOM_IN);
        else if (event.xbutton.button == 5)
          send_button_release(BUTTON_ZOOM_OUT);
      }
      break;

    case KeyPress:
      {
        KeySym sym = XLookupKeysym(&event.xkey,0);
            
        switch (sym)
          {
          case XK_F11:
            // FIXME: Shortcut
            //send_button_press(BUTTON_FULLSCREEN);
            toggle_fullscreen();
            break;

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

          case XK_a:
            send_button_press(BUTTON_ACTIONCAM);
            break;

          case XK_j:
            send_button_press(BUTTON_JOIN);
            break;

          case XK_o:
            send_button_press(BUTTON_HIDEDOTS);
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
          case XK_g:            
            send_button_press(BUTTON_GRID);
            break;
          case XK_Tab:
            send_button_press(BUTTON_TOGGLESLOWMO);
            break;
          case 65451: // FIXME: insert symbol here
          case XK_equal:
          case XK_plus:
            send_button_press(BUTTON_ZOOM_IN);
            break;
          case 65453: // FIXME: insert symbol here
          case XK_minus:
            send_button_press(BUTTON_ZOOM_OUT);
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
            std::cout << "X11Display: unhandled keypress: " << sym << " " << XK_grave << std::endl;
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
  events.push(ev); 
}

void
X11Display::send_button_release (int i)
{
  Event ev;
  ev.button.type = BUTTON_EVENT;
  ev.button.id = i;
  ev.button.pressed = false;
  events.push(ev); 
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
X11Display::save_mode()
{
#ifdef HAVE_LIBXXF86VM
  memset(&orig_modeline, 0, sizeof(orig_modeline));

  // Get the current display settings for later restore
  XF86VidModeGetModeLine(display, 
                         DefaultScreen(display),
                         &orig_dotclock,
                         &orig_modeline);

  XF86VidModeGetViewPort(display,
                         DefaultScreen(display),
                         &orig_viewport_x,
                         &orig_viewport_y);
  std::cout << "save_mode: "
            << orig_dotclock << " " 
            << orig_viewport_x << ", " << orig_viewport_y << std::endl;

#endif /* HAVE_LIBXXF86VM */
}

void
X11Display::set_fullscreen ()
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

  save_mode();

  XF86VidModeModeInfo **modes; 
  int nmodes;
  int mode_index = -1;
  if (XF86VidModeGetAllModeLines(display,
                                 DefaultScreen(display),
                                 &nmodes,&modes)) // FIXME: memleak
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
                                     DefaultScreen(display),
                                     modes[mode_index]))
            {
              fullscreen = true;

              { // Now that we have switched to the correct mode, we
                // need to position the Viewport correct to the window
                Window child_window;
                int x, y;
                // Get the windows absolute position (aka relative to
                // the root window)
                XTranslateCoordinates(display, window, DefaultRootWindow(display), 
                                      0, 0, 
                                      &x, &y, &child_window);
                XF86VidModeSetViewPort(display, DefaultScreen(display), x, y);
              }
              
              // Hijack the focus (works only till the next focus change)
              XSetInputFocus(display, window, RevertToParent, CurrentTime);

              // Capture the pointer
              if (XGrabPointer(display, window, true, 0, GrabModeAsync, GrabModeAsync, 
                               window, None, CurrentTime) != GrabSuccess)
                {
                  std::cout << "X11Display: Couldn't grab the pointer" << std::endl;
                }
            }
          else
            {
              std::cout << "X11Display: Throuble switiching to fullscreen?!" << std::endl;
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
  while (!ScreenManager::instance ()->is_finished ())
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
      ScreenManager::instance ()->run_once();
    }
}

void
X11Display::toggle_fullscreen()
{
  std::cout << "Fullscreen state: " << fullscreen << std::endl;

  if (fullscreen)
    restore_mode();
  else
    set_fullscreen();
}

void
X11Display::restore_mode ()
{
#ifdef HAVE_LIBXXF86VM
  std::cout << "X11Display::restore_mode()" << std::endl;

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

  XUngrabPointer(display, CurrentTime);

  fullscreen = false;
#endif
}

void
X11Display::set_clip_rect (int x1, int y1, int x2, int y2)
{
  XRectangle rect[1];

  rect[0].x = x1;
  rect[0].y = y1;
  rect[0].width  = x2 - x1 + 1;
  rect[0].height = y2 - y1 + 1;

  XSetClipRectangles (display, gc, 
                      0, 0, // clip origin
                      rect, 1,
                      Unsorted);
}

unsigned int
X11Display::get_color_value(const Color& color)
{
  XColor x_color;

  x_color.red   = int(color.r * 65535);
  x_color.green = int(color.g * 65535);
  x_color.blue  = int(color.b * 65535);

  XAllocColor(display, colormap, &x_color);

  return x_color.pixel;
}

XColor
X11Display::get_xcolor(const Color& color)
{
  XColor x_color;

  x_color.red   = int(color.r * 65535);
  x_color.green = int(color.g * 65535);
  x_color.blue  = int(color.b * 65535);

  XAllocColor(display, colormap, &x_color);

  return x_color;
 
}

/* EOF */
