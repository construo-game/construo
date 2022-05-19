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

#ifndef HEADER_WORLDVIEW_COMPONENT_HPP
#define HEADER_WORLDVIEW_COMPONENT_HPP

#include <memory>

#include "input_context.hpp"
#include "zoom_graphic_context.hpp"
#include "gui_component.hpp"

/** GUI Component that manages the view and the editing of the
    world, this is the main component of Construo */
class WorldViewComponent : public GUIComponent
{
public:
  enum Mode { INSERT_MODE, SELECT_MODE, ZOOM_MODE, COLLIDER_MODE };

private:
  static WorldViewComponent* instance_;

public:
  WorldViewComponent();
  ~WorldViewComponent();

  static inline WorldViewComponent* instance() { return instance_; }

  WorldViewInsertTool* get_insert_tool() { return m_insert_tool.get(); }

  void draw(GraphicContext& parent_gc) override;

  void set_mode(Mode m);
  Mode get_mode() { return m_mode; }

  bool uses_grid() const { return  m_use_grid; }
  float get_grid_size();
  float get_snap_size();

  void on_primary_button_press(geom::fpoint const& pos) override;
  void on_primary_button_release(geom::fpoint const& pos) override;

  void on_secondary_button_press(geom::fpoint const& pos) override;
  void on_secondary_button_release(geom::fpoint const& pos) override;

  void on_tertiary_button_press(geom::fpoint const& pos) override;
  void on_tertiary_button_release(geom::fpoint const& pos) override;

  void on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset) override;

  void on_scale_press(geom::fpoint const& pos) override;
  void on_grid_press(geom::fpoint const& pos) override;
  void on_duplicate_press(geom::fpoint const& pos) override;
  void on_delete_press(geom::fpoint const& pos) override;
  void on_fix_press(geom::fpoint const& pos) override;
  void on_join_press(geom::fpoint const& pos) override;

  void on_button_press(int button_id, geom::fpoint const& pos) override;

  void wheel_up(geom::fpoint const& pos) override;
  void wheel_down(geom::fpoint const& pos) override;

  void scroll_left() override;
  void scroll_right() override;
  void scroll_up() override;
  void scroll_down() override;

  float get_scale();

  /** Called once the world has changed, aka a new world got loaded */
  void on_world_change();

  bool is_at(geom::fpoint const& pos) const override { return true; }

  GCZoomState& zoom() { return m_zoom; }

private:
  GCZoomState m_zoom;

  /** If set to true, display a grid and align dots to it */
  bool m_use_grid;
  float m_grid_base_size;
  int m_grid_constant;
  float m_grid_scale_factor;
  float m_grid_snap_factor;

  /** True if the third button is currently pressed and we are in
      scroll mode */
  bool m_scrolling;

  geom::fpoint m_scroll_pos;

  geom::foffset m_offset;

  /** If the user clicks in this component, the tool decides what to
      do */
  std::unique_ptr<WorldViewSelectTool> m_select_tool;
  std::unique_ptr<WorldViewInsertTool> m_insert_tool;
  std::unique_ptr<WorldViewZoomTool> m_zoom_tool;
  std::unique_ptr<WorldViewColliderTool> m_collider_tool;
  WorldViewTool* m_current_tool;
  Mode m_mode;

public:
  WorldViewComponent(const WorldViewComponent&) = delete;
  WorldViewComponent& operator=(const WorldViewComponent&) = delete;
};

#endif

/* EOF */
