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

#include "x11_display.hpp"

#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <string.h>
#include <unistd.h>

#include <logmich/log.hpp>
#include <geom/io.hpp>

#include "construo_error.hpp"
#include "controller.hpp"
#include "cursors/cursors.hpp"
#include "screen_manager.hpp"
#include "settings.hpp"

Atom wm_delete_window;

constexpr long _NET_WM_STATE_REMOVE = 0; /* remove/unset property */
constexpr long _NET_WM_STATE_ADD = 1;    /* add/set property */
constexpr long _NET_WM_STATE_TOGGLE = 2; /* toggle property  */

X11Display::X11Display(std::string const& title, int w, int h, bool fullscreen_) :
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
  m_doublebuffer(g_settings.doublebuffer),
  m_orig_viewport_x(),
  m_orig_viewport_y(),
  m_orig_dotclock(),
  m_geometry(geom::isize(w, h)),
  m_display(),
  m_window(),
  m_colormap(),
  m_drawable(),
  m_gc(),
  m_shift_pressed (false),
  m_mouse_x(),
  m_mouse_y(),
  m_depth(),
  m_fullscreen(fullscreen_),
  m_pending_configure_event()
{
  log_info("Using X11 display");
  m_display = XOpenDisplay(NULL);
  if (!m_display) {
    throw ConstruoError("X11Display: Couldn't conncet to X server");
  }

  XSetErrorHandler([](Display* display, XErrorEvent* error_event){
    std::array<char, 1024> buffer;
    XGetErrorText(display, error_event->error_code, buffer.data(), buffer.size());

    log_error("X11 Error: {}", buffer.data());
    exit(EXIT_FAILURE);
    return 0;
  });

  int screen = DefaultScreen(m_display);
  XSetWindowAttributes attributes;

  attributes.background_pixel  = BlackPixel(m_display, screen);
  attributes.border_pixel      = WhitePixel(m_display, screen);

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
                           m_geometry.width(), m_geometry.height(), 0,
                           CopyFromParent, // depth
                           InputOutput, // class
                           nullptr /*CopyFromParent*/, // visual
                           /*CWBackPixel | CWBorderPixel |*/ CWEventMask | CWColormap,
                           &attributes);

  { // Communicate a bit with the window manager
    char* title_arr[] = { const_cast<char*>(title.c_str()) };
    XTextProperty text_property;
    XStringListToTextProperty(title_arr, 1, &text_property);

    Atom wm_name = XInternAtom(m_display, "_NET_WM_NAME", False);
    XChangeProperty(m_display, m_window, wm_name, XInternAtom(m_display, "UTF8_STRING", false), 8, PropModeReplace,
                    const_cast<unsigned char*>(reinterpret_cast<unsigned char const*>(title.c_str())),
                    static_cast<int>(title.size()));

    Atom wm_pid = XInternAtom(m_display, "_NET_WM_PID", False);
    pid_t pid = getpid();
    XChangeProperty(m_display, m_window, wm_pid, XA_CARDINAL, 32, PropModeReplace,
                    reinterpret_cast<unsigned char const*>(&pid), 1);

    if (m_fullscreen) {
      Atom wm_state = XInternAtom(m_display, "_NET_WM_STATE", False);
      Atom wm_fullscreen = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);
      XChangeProperty(m_display, m_window, wm_state, XA_ATOM, 32, PropModeReplace,
                      reinterpret_cast<unsigned char*>(&wm_fullscreen), 1);
    }

    XSizeHints size_hints;
    size_hints.x = 0;
    size_hints.y = 0;
    size_hints.flags  = PSize | PMinSize;

    size_hints.width  = m_geometry.width();
    size_hints.height = m_geometry.height();

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

  if (m_doublebuffer) {
    m_drawable = XCreatePixmap(m_display, m_window, m_geometry.width(), m_geometry.height(),
                               DefaultDepth(m_display, screen));
  } else {
    m_drawable = m_window;
  }

  XMapRaised(m_display, m_window);

  XGCValues gcv;
  gcv.foreground = 0xFFFFFF;
  gcv.background = 0x000000;

  if (g_settings.thick_lines)
    gcv.line_width = 2;
  else
    gcv.line_width = 0;

  m_gc = XCreateGC(m_display, m_window,
                   GCLineWidth | GCForeground | GCBackground,
                   &gcv);

  {
    // Visual* visual = XDefaultVisual(display, DefaultScreen(display));
    m_depth = DefaultDepth(m_display, DefaultScreen(m_display));
    if (m_depth != 16 && m_depth != 24 && m_depth != 32)
    {
      log_warn("X11Display: Warring color depth '{}' not supported, Construo will be slow!", m_depth);
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
  if (m_doublebuffer) {
    XFreePixmap(m_display, m_drawable);
  }

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
      log_error("X11Display: Unhandled cursor type: {}", static_cast<int>(cursor));
      break;
  }
}

void
X11Display::draw_lines(std::vector<Line>& lines, Color color, int wide)
{
  std::vector<XSegment> segments(lines.size());

  for (std::vector<Line>::size_type i = 0; i < lines.size(); ++i)
  {
    segments[i].x1 = static_cast<short>(lines[i].p1.x());
    segments[i].y1 = static_cast<short>(lines[i].p1.y());
    segments[i].x2 = static_cast<short>(lines[i].p2.x());
    segments[i].y2 = static_cast<short>(lines[i].p2.y());
  }

  XDrawSegments(m_display, m_drawable, m_gc, &*segments.begin(), static_cast<int>(segments.size()));
}

void
X11Display::draw_circles(std::vector<Circle>& circles, Color color)
{
  std::vector<XArc> arcs (circles.size());
  for (std::vector<Circle>::size_type i = 0; i < circles.size(); ++i)
  {
    arcs[i].x      = static_cast<short>(circles[i].pos.x() - circles[i].r);
    arcs[i].y      = static_cast<short>(circles[i].pos.y() - circles[i].r);
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
X11Display::draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawLine(m_display, m_drawable, m_gc, static_cast<int>(p1.x()), static_cast<int>(p1.y()), static_cast<int>(p2.x()), static_cast<int>(p2.y()));
}

void
X11Display::draw_fill_rect(geom::frect const& rect, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XFillRectangle(m_display, m_drawable, m_gc,
                 static_cast<int>(rect.left()), static_cast<int>(rect.top()),
                 static_cast<int>(rect.width()), static_cast<int>(rect.height()));
}

void
X11Display::draw_fill_circle(geom::fpoint const& pos, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(m_display, m_gc, get_color_value(color));
  XFillArc(m_display, m_drawable, m_gc,
           static_cast<int>(pos.x() - r), static_cast<int>(pos.y() - r),
           static_cast<int>(r * 2), static_cast<int>(r * 2), 0,
           360 * 64);
}

void
X11Display::draw_circle(geom::fpoint const& pos, float r, Color color)
{
  // FIXME: doesn't work
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawArc(m_display, m_drawable, m_gc,
           static_cast<int>(pos.x() - r), static_cast<int>(pos.y() - r),
           static_cast<int>(r * 2.0f), static_cast<int>(r * 2.0f),
           0, 360*64);
}

void
X11Display::draw_rect(geom::frect const& rect, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawRectangle(m_display, m_drawable, m_gc,
                 static_cast<int>(rect.left()), static_cast<int>(rect.top()),
                 static_cast<int>(rect.width()), static_cast<int>(rect.height()));
}

void
X11Display::draw_string(geom::fpoint const& pos, const std::string& str, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawString(m_display, m_drawable, m_gc,
              static_cast<int>(pos.x()), static_cast<int>(pos.y()),
              str.c_str (), static_cast<int>(str.length()));
}

void
X11Display::draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color)
{
  XSetForeground(m_display, m_gc, get_color_value(color));
  XDrawString(m_display, m_drawable, m_gc,
              static_cast<int>(pos.x()) - ((static_cast<int>(str.length()) * 6) / 2), static_cast<int>(pos.y()),
              str.c_str (), static_cast<int>(str.length()));
}

float
X11Display::get_mouse_x ()
{
  return static_cast<float>(m_mouse_x);
}

float
X11Display::get_mouse_y ()
{
  return static_cast<float>(m_mouse_y);
}

bool
X11Display::get_key (int key)
{
  return false;
}

void
X11Display::wait_for_events_blocking()
{
  do {
    XEvent event;

    do { // Ignore NoExpose events, as otherwise this won't block
      XNextEvent(m_display, &event);
    } while (event.type == NoExpose);

    process_event(event);
  } while (XPending(m_display) > 0);

  process_pending_events();
}

void
X11Display::wait_for_events ()
{
  while (XPending(m_display) > 0) {
    XEvent event;
    XNextEvent(m_display, &event);
    process_event(event);
  }
  process_pending_events();
}

void
X11Display::process_pending_events()
{
  if (m_pending_configure_event) {
    // Only process one ConfigureNotify per loop, otherwise the queue
    // will get filled with ever more ConfigureNotify events and cause
    // huge delays.

    geom::irect new_geometry;
    { // FIXME: Don't know how to interpret ConfigureNotify properly,
      // it does not return absolute values, but values relative to
      // something (m_pending_configure_event->above?!). This code
      // seems to get the proper values, but might do unnecessary
      // work.
      int x, y;
      Window child;
      XWindowAttributes xwa;
      XTranslateCoordinates(m_display, m_window, RootWindow(m_display, DefaultScreen(m_display)),
                            0, 0, &x, &y, &child);
      XGetWindowAttributes(m_display, m_window, &xwa);

      new_geometry = geom::irect(geom::ipoint(xwa.x + x, xwa.y + y),
                                 geom::isize(xwa.width, xwa.height));
    }

    if (m_doublebuffer) {
      if (m_geometry.width() < new_geometry.width() ||
          m_geometry.height() < new_geometry.height())
      {
        // enlarge the pixmap when necessary
        XFreePixmap(m_display, m_drawable);
        m_drawable = XCreatePixmap(m_display, m_window,
                                   new_geometry.width(),
                                   new_geometry.height(),
                                   DefaultDepth(m_display, DefaultScreen(m_display)));
      }
    }

    m_geometry = new_geometry;

    ScreenManager::instance()->set_geometry(geom::frect(m_geometry));

    m_pending_configure_event = std::nullopt;
  }
}

void
X11Display::process_event(XEvent& event)
{
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
      break;

    case ButtonPress:
      {
        // log_info("ButtonID: {} {}", event.xbutton.button, event.xbutton.state);
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
        // log_debug("ButtonID: {} {}", event.xbutton.button, event.xbutton.state);
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
            log_debug("X11Display: unhandled keypress: {}", sym);
            break;
        }
      }
      break;

    case KeyRelease:
      {
        KeySym sym = XLookupKeysym(&event.xkey, 0);

        switch (sym)
        {
          case XK_Shift_L:
          case XK_Shift_R:
            m_shift_pressed = false;
            break;
          default:
            // log_debug("X11Display: unhandled keyrelease: {}", sym);
            break;
        }
      }
      break;

    case ResizeRequest:
      log_debug("X11Display:ResizeRequest: {}x{}", event.xresizerequest.width, event.xresizerequest.height);
      break;

    case ConfigureNotify:
      m_pending_configure_event = event.xconfigure;
      break;

    case DestroyNotify:
      log_debug("Window got destroyed");
      break;

    case ClientMessage:
      log_debug("X11Display: got client message");
      // Window close request
      if (static_cast<int>(event.xclient.data.l[0]) == static_cast<int>(wm_delete_window)) {
        log_debug("Window is destroyed");
        send_button_press(BUTTON_ESCAPE);
      }
      break;

    default:
      log_debug("X11Display: Unhandled event: {}", event.type);
      break;
  }
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
  XFillRectangle(m_display, m_drawable, m_gc, 0, 0, m_geometry.width(), m_geometry.height());
}

void
X11Display::flip()
{
  if (m_doublebuffer)
  {
    // FIXME: Use another gc here
    XCopyArea(m_display, m_drawable, m_window, m_gc,
              0, 0, // source
              m_geometry.width(), m_geometry.height(),
              0, 0 // destination
      );

    XFlush(m_display);
    // FIXME: brute force sleep to limit the framerate and keep the
    // XServer responsive for other applications.
    usleep(5000);
  }
}

void
X11Display::run()
{
  while (!ScreenManager::instance ()->is_finished ())
  {
    ScreenManager::instance()->run_once(*this);

    if (Controller::instance()->is_running())
    {
      g_system_context->sleep(0); // FIXME: limit CPU usage via brute force
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
  if (m_fullscreen) {
    leave_fullscreen();
  } else {
    enter_fullscreen();
  }
}

void
X11Display::enter_fullscreen ()
{
  // Mapped windows must use XSendEvent, not XChangeProperty
  XEvent event;

  event.xclient.type = ClientMessage;
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(m_display, "_NET_WM_STATE", False);
  event.xclient.window = m_window;
  event.xclient.format = 32;
  event.xclient.data.l[0] = _NET_WM_STATE_ADD;
  event.xclient.data.l[1] = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

  if (!XSendEvent(m_display, DefaultRootWindow(m_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event)) {
    log_error("XSendEvent failure");
  }

  m_fullscreen = true;
}

void
X11Display::leave_fullscreen()
{
  // Mapped windows must use XSendEvent, not XChangeProperty
  XEvent event;

  event.xclient.type = ClientMessage;
  event.xclient.serial = 0;
  event.xclient.send_event = True;
  event.xclient.message_type = XInternAtom(m_display, "_NET_WM_STATE", False);
  event.xclient.window = m_window;
  event.xclient.format = 32;
  event.xclient.data.l[0] = _NET_WM_STATE_REMOVE;
  event.xclient.data.l[1] = XInternAtom(m_display, "_NET_WM_STATE_FULLSCREEN", False);
  event.xclient.data.l[2] = 0;
  event.xclient.data.l[3] = 0;
  event.xclient.data.l[4] = 0;

  if (!XSendEvent(m_display, DefaultRootWindow(m_display), False, SubstructureRedirectMask | SubstructureNotifyMask, &event)) {
    log_error("XSendEvent failure");
  }

  m_fullscreen = false;
}

void
X11Display::set_clip_rect(geom::frect const& rect)
{
  XRectangle xrect;

  xrect.x = static_cast<short int>(rect.left());
  xrect.y = static_cast<short int>(rect.top());
  xrect.width  = static_cast<short int>(rect.width());
  xrect.height = static_cast<short int>(rect.height());

  XSetClipRectangles(m_display, m_gc,
                     0, 0, // clip origin
                     &xrect, 1,
                     Unsorted);
}

void
X11Display::clear_clip_rect()
{
  XSetClipMask(m_display, m_gc, None);
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
