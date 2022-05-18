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

#ifndef HEADER_CONSTRUO_GUI_CHILDMANAGER_HPP
#define HEADER_CONSTRUO_GUI_CHILDMANAGER_HPP

#include <memory>
#include <string>
#include <vector>

#include "gui_component.hpp"
#include "zoom_graphic_context.hpp"

class GUIChildManager : public GUIComponent
{
public:
  GUIChildManager();
  ~GUIChildManager();

  void add(std::unique_ptr<GUIComponent>);
  void remove(GUIComponent*);
  void clear();

  template<typename T, typename... Args>
  T* create(Args&&... args) {
    std::unique_ptr<T> obj = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = obj.get();
    add(std::move(obj));
    return ptr;
  }

  void draw(GraphicContext& gc) override;
  virtual void draw_overlay(GraphicContext& gc) {}

  void on_primary_button_press(geom::fpoint const& pos) override;
  void on_primary_button_release(geom::fpoint const& pos) override;

  void on_secondary_button_click(geom::fpoint const& pos);
  void on_secondary_button_press(geom::fpoint const& pos) override;
  void on_secondary_button_release(geom::fpoint const& pos) override;

  void on_delete_press(geom::fpoint const& pos) override;
  void on_fix_press(geom::fpoint const& pos) override;

  void on_mouse_enter() override;
  void on_mouse_leave() override;

  void wheel_up(geom::fpoint const& pos) override;
  void wheel_down(geom::fpoint const& pos) override;

  void scroll_left() override;
  void scroll_right() override;
  void scroll_up() override;
  void scroll_down() override;

  void on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset) override;

private:
  GUIComponent* find_component_at(geom::fpoint const& pos) const;

private:
  std::vector<std::unique_ptr<GUIComponent>> m_components;
  GUIComponent* m_current_component;

public:
  GUIChildManager(const GUIChildManager&) = delete;
  GUIChildManager& operator=(const GUIChildManager&) = delete;
};

#endif

/* EOF */
