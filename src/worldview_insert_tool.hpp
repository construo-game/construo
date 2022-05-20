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

#ifndef HEADER_CONSTRUO_WORLDVIEW_INSERT_TOOL_HPP
#define HEADER_CONSTRUO_WORLDVIEW_INSERT_TOOL_HPP

#include "worldview_tool.hpp"

class WorldViewInsertTool : public WorldViewTool
{
public:
  WorldViewInsertTool(WorldViewWidget& worldview);
  ~WorldViewInsertTool();

  void draw_background(ZoomGraphicContext& gc) override;
  void draw_foreground(ZoomGraphicContext& gc) override;

  void on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset) override;

  void on_primary_button_press(geom::fpoint const& pos) override;
  void on_primary_button_release(geom::fpoint const& pos) override;

  void on_secondary_button_press(geom::fpoint const& pos) override;
  void on_secondary_button_release(geom::fpoint const& pos) override;

  void on_delete_press(geom::fpoint const& pos) override;
  void on_fix_press(geom::fpoint const& pos) override;

private:
  /** The last particle that got inserted or marked by a click, it is
      used as the first particle of the newly created spring */
  Particle* m_previous_particle;

  Spring* m_hover_spring;
  Particle* m_hover_particle;

public:
  WorldViewInsertTool(const WorldViewInsertTool&) = delete;
  WorldViewInsertTool& operator=(const WorldViewInsertTool&) = delete;
};

#endif

/* EOF */
