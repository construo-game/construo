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

#ifndef HEADER_CONSTRUO_GUI_WIDGET_HPP
#define HEADER_CONSTRUO_GUI_WIDGET_HPP

#include <geom/rect.hpp>

#include "fwd.hpp"

namespace construo {

/** A thing that is under the controll of the GUIManager */
class GUIWidget
{
public:
  GUIWidget() :
    m_geometry()
  {}
  virtual ~GUIWidget() {}

  virtual void draw(GraphicContext& gc) = 0;

  /** @return true if the widget is present at the given location */
  virtual bool is_at(geom::fpoint const& pos) const;

  virtual void set_geometry(geom::frect const& geometry) {
    m_geometry = geometry;
  }

  geom::frect geometry() const {
    return m_geometry;
  }

  virtual void on_primary_button_press(geom::fpoint const& pos) {}
  virtual void on_primary_button_release(geom::fpoint const& pos) {}

  virtual void on_secondary_button_press(geom::fpoint const& pos) {}
  virtual void on_secondary_button_release(geom::fpoint const& pos) {}

  virtual void on_tertiary_button_press(geom::fpoint const& pos) {}
  virtual void on_tertiary_button_release(geom::fpoint const& pos) {}

  // FIXME: Join these under some generic event handling, like:
  // void on_button_press(int button_id, int x, int y);
  virtual void on_scale_press(geom::fpoint const& pos) {}
  virtual void on_grid_press(geom::fpoint const& pos) {}
  virtual void on_duplicate_press(geom::fpoint const& pos) {}
  virtual void on_delete_press(geom::fpoint const& pos) {}
  virtual void on_fix_press(geom::fpoint const& pos) {}
  virtual void on_join_press(geom::fpoint const& pos) {}

  virtual void on_mouse_enter() {}
  virtual void on_mouse_leave() {}

  virtual void wheel_up(geom::fpoint const& pos) {}
  virtual void wheel_down(geom::fpoint const& pos) {}

  virtual void on_button_press(int button_id, geom::fpoint const& pos) {}

  virtual void scroll_left() {}
  virtual void scroll_right() {}
  virtual void scroll_up() {}
  virtual void scroll_down() {}

  virtual void on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset) {}

protected:
  geom::frect m_geometry;

public:
  GUIWidget(const GUIWidget&) = delete;
  GUIWidget& operator=(const GUIWidget&) = delete;
};

} // namespace construo

#endif

/* EOF */
