// Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_GLUT_DISPLAY_HPP
#define HEADER_CONSTRUO_GLUT_DISPLAY_HPP

#include <unordered_map>

#include <geom/rect.hpp>
#include <geom/size.hpp>

#include "root_graphic_context.hpp"
#include "input_context.hpp"

class GlutDisplay : public RootGraphicContext,
                    public InputContext
{
public:
  GlutDisplay(std::string const& title, int width, int height, int fullscreen);
  virtual ~GlutDisplay();

  void draw_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_circles(std::vector<Circle>& circles, Color color) override;
  void draw_fill_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_fill_rect(geom::frect const& rect, Color color) override;
  void draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide = 0) override;
  void draw_lines(std::vector<Line>& lines, Color color, int wide = 0) override;
  void draw_rect(geom::frect const& rect, Color color) override;
  void draw_string(geom::fpoint const& pos, const std::string& str, Color color) override;
  void draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color) override;

  void clear() override;
  void flip() override;

  bool get_fullscreen() { return m_is_fullscreen; }

  geom::frect geometry() const override {
    return geom::frect(geom::fsize(m_size));
  }

  geom::fpoint get_mouse_pos() const override;

  void run();
  void set_cursor_real(CursorType) override;
  void reshape_func(int w, int h);
  void display_func();
  void mouse_func(int button, int button_state, int x, int y);
  void idle_func();
  void keyboard_func(unsigned char key, int x, int y);
  void special_func(int key, int x, int y);
  void mouse_motion_func(int x, int y);

  void set_clip_rect(geom::frect const& rect) override;
  void clear_clip_rect() override;

  void push_quick_draw() override;
  void pop_quick_draw() override;

  void enter_fullscreen() override;
  void leave_fullscreen() override;
  void toggle_fullscreen() override;

  void bind_key(unsigned char key, Action action);

private:
  /** Position of the glut window, used in fullscreen to allow a
      restore */
  geom::irect m_window_geometry;

  /** The actual size of the current screen/window */
  geom::isize m_size;

  geom::ipoint m_mouse_pos;

  bool m_block;

  int m_update_display;

  bool m_is_fullscreen;

  std::unordered_map<unsigned char, Action> m_key_bindings;

public:
  GlutDisplay(const GlutDisplay&) = delete;
  GlutDisplay& operator=(const GlutDisplay&) = delete;
};

#endif

/* EOF */
