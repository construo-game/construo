// Construo - A wire-frame construction game
// Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <iostream>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <string.h>

#include "cursors/cursors.hpp"

#include "construo_error.hpp"
#include "x11_display.hpp"
#include "settings.hpp"
#include "construo_main.hpp"

#include "controller.hpp"
#include "screen_manager.hpp"

extern ConstruoMain* construo_main;
Atom wm_delete_window;

X11Display::X11Display(int w, int h, bool fullscreen_) :
  m_cursor_scroll(),
  m_cursor_scroll_pix(),
  m_cursor_scroll_mask(),
  m_cursor_zoom(),
  m_cursor_zoom_pix(),
  m_cursor_zoom_mask(),
  m_cursor_insert(),
  m_cursor_insert_pix(),
  m_cursor_insert_mask(),
  m_cursor_select(),
  m_cursor_select_pix(),
  m_cursor_select_mask(),
  m_cursor_collider(),
  m_cursor_collider_pix(),
  m_cursor_collider_mask(),
  m_doublebuffer(settings.doublebuffer),
#ifdef HAVE_LIBXXF86VM
  m_orig_modeline(),
#endif
  m_orig_viewport_x(),
  m_orig_viewport_y(),
  m_orig_dotclock(),
  m_width(w),
  m_height(h),
  m_display(),
  m_window(),
  m_colormap(),
  m_drawable(),
  m_gc(),
  m_shift_pressed (false),
  m_mouse_x(),
  m_mouse_y(),
  m_depth(),
  m_fullscreen (fullscreen_)
{
#ifndef HAVE_LIBXXF86VM
  m_fullscreen = false;
  std::cout << "X11Display: libXxf86vm missing, fullscreen support not\n"
            << "            available, please recompile." << std::endl;
#endif

  std::cout << "Using X11 display" << std::endl;
  m_display = XOpenDisplay(NULL);

  if (!m_display) {
    throw ConstruoError("X11Display: Couldn't conncet to X server");
  }

  int screen = DefaultScreen(m_display);
  XSetWindowAttributes attributes;

  attributes.background_pixel  = BlackPixel(m_display, screen);
  attributes.border_pixel      = WhitePixel(m_display, screen);

  if (m_fullscreen)
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
    // ResizeRedirectMask   |
    ExposureMask;

  m_colormap = DefaultColormap(m_display, screen);
  attributes.colormap = m_colormap;
  m_window = XCreateWindow(m_display, RootWindow(m_display, screen),
                           0,0, // position
                           m_width, m_height, 0,
                           CopyFromParent, // depth
                           InputOutput, // class
                           nullptr /*CopyFromParent*/, // visual
                           CWOverrideRedirect | CWBackPixel | CWBorderPixel | CWEventMask | CWColormap,
                           &attributes);

  { // Communicate a bit with the window manager
    char *title = const_cast<char*>(construo_main->get_title());

    XTextProperty text_property;
    XStringListToTextProperty(&title, 1, &text_property);

    XSizeHints size_hints;
    size_hints.x = 0;
    size_hints.y = 0;
    size_hints.flags  = PSize | PMinSize;

    size_hints.width  = m_width;
    size_hints.height = m_height;

    size_hints.min_width  = 0;
    size_hints.min_height = 0;

    size_hints.max_width  = 0;
    size_hints.max_height = 0;

    XSetWMProperties(
      m_display,
      m_window,
      &text_property, // window name
      &text_property, // icon name
      nullptr, // argv
      0, // argc
      &size_hints,
      nullptr, // XWMHints
      nullptr // class_hinst
      );

    // Set WM_DELETE_WINDOW atom in WM_PROTOCOLS property (to get window_delete requests).
    wm_delete_window = XInternAtom(m_display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(m_display, m_window, &wm_delete_window, 1);
  }

  if (m_doublebuffer)
    m_drawable = XCreatePixmap(m_display, m_window, m_width, m_height,
                               DefaultDepth(m_display, screen));
  else
    m_drawable = m_window;

  XMapRaised(m_display, m_window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;

  if (settings.thick_lines)
    gcv.line_width = 2;
  else
    gcv.line_width = 0;

  m_gc = XCreateGC(m_display, m_window,
                   GCLineWidth | GCForeground | GCBackground,
                   &gcv);

  if (m_fullscreen)
    enter_fullscreen();

  {
    // Visual* visual = XDefaultVisual(display, DefaultScreen(display));
    m_depth = DefaultDepth(m_display, DefaultScreen(m_display));
    if (m_depth != 16 && m_depth != 24 && m_depth != 32)
    {
      std::cout << "X11Display: Warring color depth '" << m_depth
                << "' not supported, Construo will be slow!" << std::endl;
    }
  }

  {
    // Black&White
    XColor cursor_fg = get_xcolor(Color(1.0f, 1.0f, 1.0f));
    XColor cursor_bg = get_xcolor(Color(0, 0, 0));

    m_cursor_scroll_pix = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_scroll_bits),
                                                cursor_scroll_width, cursor_scroll_height);
    m_cursor_scroll_mask = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_scroll_mask_bits),
                                                 cursor_scroll_width, cursor_scroll_height);
    m_cursor_scroll = XCreatePixmapCursor(m_display, m_cursor_scroll_pix, m_cursor_scroll_mask, &cursor_bg, &cursor_fg,
                                          cursor_scroll_x_hot, cursor_scroll_y_hot);

    m_cursor_zoom_pix =  XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_zoom_bits),
                                               cursor_zoom_width, cursor_zoom_height);
    m_cursor_zoom_mask = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_zoom_mask_bits),
                                               cursor_zoom_width, cursor_zoom_height);
    m_cursor_zoom = XCreatePixmapCursor(m_display, m_cursor_zoom_pix, m_cursor_zoom_mask, &cursor_bg, &cursor_fg,
                                        cursor_zoom_x_hot, cursor_zoom_y_hot);

    m_cursor_insert_pix =  XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_insert_bits),
                                                 cursor_insert_width, cursor_insert_height);
    m_cursor_insert_mask = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_insert_mask_bits),
                                                 cursor_insert_width, cursor_insert_height);
    m_cursor_insert = XCreatePixmapCursor(m_display, m_cursor_insert_pix, m_cursor_insert_mask, &cursor_bg, &cursor_fg,
                                          cursor_insert_x_hot, cursor_insert_y_hot);

    m_cursor_select_pix =  XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_select_bits),
                                                 cursor_select_width, cursor_select_height);
    m_cursor_select_mask = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_select_mask_bits),
                                                 cursor_select_width, cursor_select_height);
    m_cursor_select = XCreatePixmapCursor(m_display, m_cursor_select_pix, m_cursor_select_mask, &cursor_bg, &cursor_fg,
                                          cursor_select_x_hot, cursor_select_y_hot);

    m_cursor_collider_pix =  XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_collider_bits),
                                                   cursor_collider_width, cursor_collider_height);
    m_cursor_collider_mask = XCreateBitmapFromData(m_display, m_window, reinterpret_cast<char*>(cursor_collider_mask_bits),
                                                   cursor_collider_width, cursor_collider_height);
    m_cursor_collider = XCreatePixmapCursor(m_display, m_cursor_collider_pix, m_cursor_collider_mask, &cursor_bg, &cursor_fg,
                                            cursor_collider_x_hot, cursor_collider_y_hot);
  }

  set_cursor(CURSOR_INSERT);
}

X11Display::~X11Display()
{
  //std::cout << "Closing X11 display" << std::endl;

  if (m_fullscreen)
    {
      //std::cout << "X11Display: Restoring video mode" << std::endl;
      leave_fullscreen ();
    }

  if (m_doublebuffer)
    XFreePixmap(m_display, m_drawable);

  XDestroyWindow(m_display, m_window);
  XCloseDisplay(m_display);
}

void
X11Display::set_cursor_real(CursorType cursor)
{
  switch(cursor)
    {
    case CURSOR_INSERT:
      XDefineCursor(m_display, m_window, m_cursor_insert);
      break;
    case CURSOR_SCROLL:
      XDefineCursor(m_display, m_window, m_cursor_scroll);
      break;
    case CURSOR_ZOOM:
      XDefineCursor(m_display, m_window, m_cursor_zoom);
      break;
    case CURSOR_COLLIDER:
      XDefineCursor(m_display, m_window, m_cursor_collider);
      break;
    case CURSOR_SELECT:
      XDefineCursor(m_display, m_window, m_cursor_select);
      break;
    default:
      std::cout << "X11Display: Unhandled cursor type: " << static_cast<int>(cursor) << std::endl;
      break;
    }
}

void
X11Display::draw_lines(std::vector<Line>& lines, Color color, int wide)
{
  std::vector<XSegment> segments(lines.size());

  for (std::vector<Line>::size_type i = 0; i < lines.size(); ++i)
    {
      segments[i].x1 = static_cast<short>(lines[i].x1);
      segments[i].y1 = static_cast<short>(lines[i].y1);
      segments[i].x2 = static_cast<short>(lines[i].x2);
      segments[i].y2 = static_cast<short>(lines[i].y2);
    }

  XDrawSegments(m_display, m_drawable, m_gc, &*segments.begin(), static_cast<int>(segments.size()));
}

void
X11Display::draw_circles(std::vector<Circle>& circles, Color color)
{
  std::vector<XArc> arcs (circles.size());
  for (std::vector<Circle>::size_type i = 0; i < circles.size(); ++i)
    {
      arcs[i].x      = static_cast<short>(circles[i].x - circles[i].r);
      arcs[i].y      = static_cast<short>(circles[i].y - circles[i].r);
      arcs[i].width  = static_cast<short>(2 * circles[i].r);
      arcs[i].height = static_cast<short>(2 * circles[i].r);
      arcs[i].angle1 = 0;
      arcs[i].angle2 = 360 * 64;
    }

  XSetForeground(m_display, m_gc, get_color_value(color));
  XFillArcs(m_display, m_drawable, m_gc,
            &*arcs.begin(), static_cast<int>(arcs.size()));
}

void
X11Display::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawLine(m_display, m_drawable, m_gc, static_cast<int>(x1), static_cast<int>(y1), static_cast<int>(x2), static_cast<int>(y2));
}

void
X11Display::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XFillRectangle(m_display, m_drawable, m_gc,
                  static_cast<int>(x1), static_cast<int>(y1),
                  static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));
}

void
X11Display::draw_fill_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(m_display, m_gc, get_color_value(color));
  XFillArc(m_display, m_drawable, m_gc,
           static_cast<int>(x-r), static_cast<int>(y-r),
           static_cast<int>(r*2), static_cast<int>(r*2), 0,
           360*64);
}

void
X11Display::draw_circle(float x, float y, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawArc(m_display, m_drawable, m_gc, static_cast<int>(x-r), static_cast<int>(y-r), static_cast<int>(r*2.0f), static_cast<int>(r*2.0f), 0, 360*64);
}

void
X11Display::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawRectangle(m_display, m_drawable, m_gc,
                  static_cast<int>(x1), static_cast<int>(y1),
                  static_cast<int>(x2 - x1), static_cast<int>(y2 - y1));
}

void
X11Display::draw_string(float x, float y, const std::string& str, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawString(m_display, m_drawable, m_gc, static_cast<int>(x), static_cast<int>(y), str.c_str (), static_cast<int>(str.length()));
}

void
X11Display::draw_string_centered(float x, float y, const std::string& str, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawString(m_display, m_drawable, m_gc,
               static_cast<int>(x) - ((static_cast<int>(str.length()) * 6) / 2), static_cast<int>(y),
               str.c_str (), static_cast<int>(str.length()));
}

int
X11Display::get_mouse_x ()
{
  return m_mouse_x;
}

int
X11Display::get_mouse_y ()
{
  return m_mouse_y;
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
    while (read_event() == false);
  } while (XPending(m_display) > 0);
}

void
X11Display::wait_for_events ()
{
  while (XPending(m_display) > 0)
    {
      read_event ();
    }
}

bool
X11Display::read_event ()
{
  XEvent event;

  XNextEvent(m_display, &event);

  switch (event.type)
  {
    case MotionNotify:
      m_mouse_x = event.xmotion.x;
      m_mouse_y = event.xmotion.y;
      break;

    case Expose:
      if (event.xexpose.count == 0) {
        flip();
      }
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

          case XK_s:
            send_button_press(BUTTON_SCALE);
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
            m_shift_pressed = true;
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
            m_shift_pressed = false;
            break;
          default:
            //std::cout << "X11Display: unhandled keyrelease: " << sym << " " << XK_f << std::endl;
            break;
        }
      }
      break;

    case ResizeRequest:
      std::cout << "X11Display:ResizeRequest: "
                << event.xresizerequest.width << "x" << event.xresizerequest.height << std::endl;
      break;

    case ConfigureNotify:
      if ((false)) {
        std::cout << "X11Display:ConfigureNotify: "
                  << event.xconfigure.width << "x" << event.xconfigure.height
                  << "+" << event.xconfigure.x << "+" << event.xconfigure.y << std::endl;
      }

      m_width = event.xconfigure.width;
      m_height = event.xconfigure.height;

      ScreenManager::instance()->resize(m_width, m_height);
      break;

    case DestroyNotify:
      std::cout << "Window got destroyed" << std::endl;
      break;

    case ClientMessage:
      std::cout << "X11Display: got client message" << std::endl;
      // Window close request
      if (static_cast<int>(event.xclient.data.l[0]) == static_cast<int>(wm_delete_window)) {
        std::cout << "Window is destroyed" << std::endl;
        send_button_press(BUTTON_ESCAPE);
      }
      break;

    default:
      //std::cout << "X11Display: Unhandled event: " << event.type << std::endl;
      break;
  }
  return true;
}

void
X11Display::send_load_or_save(int n)
{
  if (m_shift_pressed)
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
  XSetForeground(m_display, m_gc, 0x000000);
  XFillRectangle(m_display, m_drawable, m_gc, 0, 0, m_width, m_height);
}

void
X11Display::flip ()
{
  if (m_doublebuffer)
    {
      // FIXME: Use another gc here
      XCopyArea(m_display, m_drawable, m_window, m_gc,
                 0, 0, // source
                 m_width, m_height,
                 0, 0 // destination
                 );
      //XFlush(m_display);
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
X11Display::enter_fullscreen ()
{
#ifndef HAVE_LIBXXF86VM
  std::cout << "X11Display: libXxf86vm missing, fullscreen support not\n"
            << "            available, please recompile." << std::endl;
#else
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

                // FIXME: This won't work if the window is partly outside of the screen
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
              if (XGrabPointer(display, window, True, 0, GrabModeAsync, GrabModeAsync,
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
          XUnmapWindow(display, window);
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
      ScreenManager::instance ()->run_once();

      if (Controller::instance()->is_running())
        {
          system_context->sleep (0); // FIXME: limit CPU usage via brute force
          wait_for_events();
        }
      else
        {
          wait_for_events_blocking();
        }
    }
}

void
X11Display::toggle_fullscreen()
{
  //std::cout << "Fullscreen state: " << fullscreen << std::endl;

  if (m_fullscreen)
    leave_fullscreen();
  else
    enter_fullscreen();
}

void
X11Display::leave_fullscreen()
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

  rect[0].x = static_cast<short int>(x1);
  rect[0].y = static_cast<short int>(y1);
  rect[0].width  = static_cast<short int>(x2 - x1 + 1);
  rect[0].height = static_cast<short int>(y2 - y1 + 1);

  XSetClipRectangles(m_display, m_gc,
                      0, 0, // clip origin
                      rect, 1,
                      Unsorted);
}

unsigned int
X11Display::get_color_value(const Color& color)
{
  switch (m_depth)
    {
    case 24:
    case 32:
      return color.get_as_rrggbb();
    case 16:
      return static_cast<int>(31 * color.b) | (static_cast<int>((63 * color.g)) << 5) | (static_cast<int>((31 * color.r)) << 11);
    default:
      { // This is extremly slow!
        XColor x_color;

        x_color.red   = static_cast<unsigned short>(color.r * 65535);
        x_color.green = static_cast<unsigned short>(color.g * 65535);
        x_color.blue  = static_cast<unsigned short>(color.b * 65535);

        XAllocColor(m_display, m_colormap, &x_color);

        return static_cast<unsigned int>(x_color.pixel);
      }
      break;
    }
}

XColor
X11Display::get_xcolor(const Color& color)
{
  XColor x_color;

  x_color.red   = static_cast<unsigned short>(color.r * 65535);
  x_color.green = static_cast<unsigned short>(color.g * 65535);
  x_color.blue  = static_cast<unsigned short>(color.b * 65535);

  XAllocColor(m_display, m_colormap, &x_color);

  return x_color;

}

/* EOF */
