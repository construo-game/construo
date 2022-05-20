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

#ifndef HEADER_CONSTRUO_WORLDVIEW_SELECT_TOOL_HPP
#define HEADER_CONSTRUO_WORLDVIEW_SELECT_TOOL_HPP

#include "fwd.hpp"
#include "selection.hpp"
#include "worldview_tool.hpp"

namespace construo {

class WorldViewSelectTool : public WorldViewTool
{
public:
  WorldViewSelectTool(WorldViewWidget& worldview);
  ~WorldViewSelectTool();

  void deactivate() override;

  void draw_background(ZoomGraphicContext& gc) override;
  void draw_foreground(ZoomGraphicContext& gc) override;

  void on_primary_button_press(geom::fpoint const& pos) override;
  void on_primary_button_release(geom::fpoint const& pos) override;

  void on_secondary_button_press(geom::fpoint const& pos) override;
  void on_secondary_button_release(geom::fpoint const& pos) override;

  void on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset) override;

  void on_button_press(int button_id, geom::fpoint const& pos) override;

  void on_scale_press(geom::fpoint const& pos) override;
  void on_duplicate_press(geom::fpoint const& pos) override;
  void on_delete_press(geom::fpoint const& pos) override;
  void on_fix_press(geom::fpoint const& pos) override;
  void on_join_press(geom::fpoint const& pos) override;

private:
  enum Mode {
    GETTING_SELECTION_MODE,
    MOVING_SELECTION_MODE,
    ROTATING_SELECTION_MODE,
    SCALING_SELECTION_MODE,
    IDLE_MODE
  };

private:
  Selection m_selection;
  Mode m_mode;

  /** The start position of a click & drap operation (aka move or
      rotate), in world coordinates */
  geom::fpoint m_click_pos;

  /** The difference the selection was moved on the last mouse move */
  glm::vec2 m_move_diff;

  /** The particle, by which is selection grabbed */
  Particle* m_move_current_particle;

  /** The center of a rotation */
  glm::vec2 m_rotate_center;

  /** The center of a scalation */
  glm::vec2 m_scale_center;
  float m_old_scale_factor;

public:
  WorldViewSelectTool(const WorldViewSelectTool&) = delete;
  WorldViewSelectTool& operator=(const WorldViewSelectTool&) = delete;
};

} // namespace construo

#endif

/* EOF */
