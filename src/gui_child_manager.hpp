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
#include <vector>
#include <string>
#include "zoom_graphic_context.hpp"
#include "gui_component.hpp"

class GUIChildManager : public GUIComponent
{
public:
  GUIChildManager(float x, float y, float width, float height);
  GUIChildManager();
  ~GUIChildManager();

  void add(GUIComponent*);
  void remove(GUIComponent*);
  void replace(GUIComponent* old_comp, GUIComponent* new_comp);

  template<typename T, typename... Args>
  T* create(Args&&... args) {
    std::unique_ptr<T> obj = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = obj.get();
    add(obj.release()); // FIXME: mem leak
    return ptr;
  }

  void draw(GraphicContext& gc) override;
  virtual void draw_overlay(GraphicContext& gc) {}

  void on_primary_button_press(float x, float y) override;
  void on_primary_button_release(float x, float y) override;

  void on_secondary_button_click(float x, float y);
  void on_secondary_button_press(float x, float y) override;
  void on_secondary_button_release(float x, float y) override;

  void on_delete_press(float x, float y) override;
  void on_fix_press(float x, float y) override;

  virtual void on_mouse_enter() override;
  virtual void on_mouse_leave() override;

  void wheel_up(float x, float y) override;
  void wheel_down(float x, float y) override;

  void scroll_left() override;
  void scroll_right() override;
  void scroll_up() override;
  void scroll_down() override;

  void on_mouse_move(float x, float y, float of_x, float of_y) override;

private:
  GUIComponent* find_component_at(float x, float y);

private:
  std::vector<GUIComponent*> m_components;
  GUIComponent* m_current_component;

public:
  GUIChildManager(const GUIChildManager&) = delete;
  GUIChildManager& operator=(const GUIChildManager&) = delete;
};

#endif

/* EOF */
