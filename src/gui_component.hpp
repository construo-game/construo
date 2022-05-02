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

#ifndef HEADER_CONSTRUO_GUI_COMPONENT_HPP
#define HEADER_CONSTRUO_GUI_COMPONENT_HPP

class GraphicContext;
class GUIManager;

/** A thing that is under the controll of the GUIManager */
class GUIComponent
{
public:
  GUIComponent() :
    m_x(0),
    m_y(0),
    m_width(0),
    m_height(0)
  {}
  GUIComponent (float x_, float y_, float width_, float height_) :
    m_x(x_),
    m_y(y_),
    m_width (width_),
    m_height(height_)
  {}

  virtual ~GUIComponent() {}

  virtual void draw(GraphicContext& gc) = 0;

  /** @return true if the component is present at the given location */
  virtual bool is_at(float x, float y);

  void set_geometry(float x, float y, float width, float height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
  }
  void set_position(float x, float y) { m_x = x, m_y = y; }
  void set_width(float w) { m_width = w; }
  void set_height(float h) { m_height = h; }
  float get_x_pos() { return m_x; }
  float get_y_pos() { return m_y; }
  float get_width() { return m_width; }
  float get_height() { return m_height; }

  virtual void on_primary_button_press(float x, float y) {}
  virtual void on_primary_button_release(float x, float y) {}

  virtual void on_secondary_button_press(float x, float y) {}
  virtual void on_secondary_button_release(float x, float y) {}

  virtual void on_tertiary_button_press(float x, float y) {}
  virtual void on_tertiary_button_release(float x, float y) {}

  // FIXME: Join these under some generic event handling, like:
  // void on_button_press(int button_id, int x, int y);
  virtual void on_scale_press(float x, float y) {}
  virtual void on_grid_press(float x, float y) {}
  virtual void on_duplicate_press(float x, float y) {}
  virtual void on_delete_press(float x, float y) {}
  virtual void on_fix_press(float x, float y) {}
  virtual void on_join_press(float x, float y) {}

  virtual void on_mouse_enter() {}
  virtual void on_mouse_leave() {}

  virtual void wheel_up(float x, float y) {}
  virtual void wheel_down(float x, float y) {}

  virtual void on_button_press(int button_id, float x, float y) {}

  virtual void scroll_left() {}
  virtual void scroll_right() {}
  virtual void scroll_up() {}
  virtual void scroll_down() {}

  virtual void on_mouse_move(float x, float y, float of_x, float of_y) {}

protected:
  float m_x;
  float m_y;
  float m_width;
  float m_height;

public:
  GUIComponent(const GUIComponent&) = delete;
  GUIComponent& operator=(const GUIComponent&) = delete;
};

#endif

/* EOF */
