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

#ifndef HEADER_X11_DISPLAY_HPP
#define HEADER_X11_DISPLAY_HPP

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <optional>
#include <unordered_map>

#include <geom/rect.hpp>

#include "math.hpp"
#include "root_graphic_context.hpp"
#include "input_context.hpp"

#define X11_FULLSCREEN_MODE true
#define X11_WINDOW_MODE     false

struct KeySequence
{
  unsigned int m_modifier;
  KeySym m_key;

  KeySequence(KeySym key) :
    m_modifier(0),
    m_key(key)
  {}

  KeySequence(unsigned int modifier, KeySym key) :
    m_modifier(modifier),
    m_key(key)
  {}

  bool operator==(KeySequence const& other) const = default;
};

template<>
struct std::hash<KeySequence>
{
  std::size_t operator()(KeySequence const& key) const noexcept {
    return std::hash<unsigned int>{}(key.m_modifier) ^ std::hash<KeySym>{}(key.m_key);
  }
};

/** X11Display driver */
namespace construo {

class X11Display : public RootGraphicContext,
                   public InputContext
{
public:
  X11Display(std::string const& title, int w, int h, bool fullscreen_);
  virtual ~X11Display();

  // Graphic Context stuff
  void draw_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_circles(std::vector<Circle>& circles, Color color) override;
  void draw_fill_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_fill_rect(geom::frect const& rect, Color color) override;
  void draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide = 0) override;
  void draw_lines(std::vector<Line>& lines, Color color, int wide = 0) override;
  void draw_rect(geom::frect const& rect, Color color) override;
  void draw_string(geom::fpoint const& pos, const std::string& str, Color color) override;
  void draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color) override;

  geom::frect geometry() const override { return geom::frect(m_geometry); }

  void toggle_fullscreen() override;

  void clear() override;

  /** Flip the double buffered display */
  void flip() override;

  void enter_fullscreen() override;
  void leave_fullscreen() override;

  // Input Context stuff
  geom::fpoint get_mouse_pos() const override;

  /** Waits for events to come in, blocks until new events are available */
  void wait_for_events_blocking();

  void wait_for_events();

  void run();

  void set_clip_rect(geom::frect const& rect) override;
  void clear_clip_rect() override;

  unsigned int get_color_value(const Color& color);
  XColor get_xcolor(const Color& color);

  void set_cursor_real(CursorType cursor) override;

  void bind_key(KeySequence const& key, Action action);

private:
  void process_event(XEvent& event);
  void process_pending_events();
  void send_button_press(Action action);
  void send_button_release(Action action);

  /** Save the current visual mode for later restoration after leaving
      fullscreen */
  void save_mode();

private:
  Cursor m_cursor_scroll;
  Pixmap m_cursor_scroll_pix;
  Pixmap m_cursor_scroll_mask;

  Cursor m_cursor_zoom;
  Pixmap m_cursor_zoom_pix;
  Pixmap m_cursor_zoom_mask;

  Cursor m_cursor_insert;
  Pixmap m_cursor_insert_pix;
  Pixmap m_cursor_insert_mask;

  Cursor m_cursor_select;
  Pixmap m_cursor_select_pix;
  Pixmap m_cursor_select_mask;

  Cursor m_cursor_collider;
  Pixmap m_cursor_collider_pix;
  Pixmap m_cursor_collider_mask;

  bool m_doublebuffer;

  int m_orig_viewport_x;
  int m_orig_viewport_y;
  int m_orig_dotclock;

  geom::irect m_geometry;
  Display* m_display;
  Window m_window;
  Colormap m_colormap;
  Drawable m_drawable;
  GC m_gc;

  geom::ipoint m_mouse_pos;

  /** Color Depth of the Display */
  int m_depth;

  /** true if display is in fullscreen mode, false for window mode */
  bool m_fullscreen;

  std::optional<XConfigureEvent> m_pending_configure_event;

  std::unordered_map<KeySequence, Action> m_key_bindings;

public:
  X11Display(const X11Display&) = delete;
  X11Display& operator=(const X11Display&) = delete;
};

} // namespace construo

#endif

/* EOF */
