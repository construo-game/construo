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

#ifndef HEADER_CONSTRUO_WORLDVIEW_COLLIDER_TOOL_HPP
#define HEADER_CONSTRUO_WORLDVIEW_COLLIDER_TOOL_HPP

#include <glm/glm.hpp>

#include "worldview_tool.hpp"

class Collider;

class WorldViewColliderTool : public WorldViewTool
{
public:
  WorldViewColliderTool(WorldViewComponent& worldview);
  ~WorldViewColliderTool();

  void draw_background (ZoomGraphicContext& gc) override;
  void draw_foreground (ZoomGraphicContext& gc) override;

  void on_primary_button_press (float x, float y) override;
  void on_primary_button_release (float x, float y) override;

  void on_secondary_button_press (float x, float y) override;
  void on_secondary_button_release (float x, float y) override;

  void on_mouse_move (float x, float y, float of_x, float of_y) override;

private:
  /** Get the collider on the given world cooridnates */
  Collider* get_collider (const glm::vec2&);

private:
  bool m_creating_rect;
  glm::vec2 m_click_pos;

  Collider* m_to_delete_collider;
  Collider* m_move_collider;

public:
  WorldViewColliderTool(const WorldViewColliderTool&) = delete;
  WorldViewColliderTool& operator=(const WorldViewColliderTool&) = delete;
};

#endif

/* EOF */
