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

#include "worldview_collider_tool.hpp"

#include <logmich/log.hpp>

#include "construo.hpp"
#include "input_context.hpp"
#include "controller.hpp"
#include "colors.hpp"
#include "world.hpp"
#include "world_renderer.hpp"
#include "world_gui_manager.hpp"
#include "worldview_widget.hpp"

namespace construo {

WorldViewColliderTool::WorldViewColliderTool(WorldViewWidget& worldview) :
  WorldViewTool(worldview),
  m_creating_rect(false),
  m_click_pos(0.0f, 0.0f),
  m_to_delete_collider(nullptr),
  m_move_collider(nullptr)
{
}

WorldViewColliderTool::~WorldViewColliderTool()
{
}

void
WorldViewColliderTool::draw_background(ZoomGraphicContext& gc)
{
}

void
WorldViewColliderTool::draw_foreground(ZoomGraphicContext& gc)
{
  World const& world = Controller::instance()->get_world();

  geom::fpoint const mouse_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

  if (Collider* collider = world.find_collider(mouse_pos)) {
    WorldRenderer::draw_collider_highlight(gc, *collider);
  }

  if (m_creating_rect)
  {
    gc.draw_rect(geom::normalize(geom::frect(m_click_pos, mouse_pos)), g_style.selection_rect);
  }
}

void
WorldViewColliderTool::on_primary_button_press (geom::fpoint const& pos)
{
  World const& world = Controller::instance()->get_world();

  WorldGUIManager::instance()->grab_mouse(m_worldview);

  m_click_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

  if ((m_move_collider = world.find_collider(m_click_pos)) != nullptr)
  {
    // click_pos Offset, not position
    m_click_pos = geom::fpoint(m_click_pos.as_vec() - m_move_collider->get_pos());
    m_creating_rect = false;
    Controller::instance()->push_undo();
  }
  else
  {
    Controller::instance()->push_undo();
    m_creating_rect = true;
  }
}

void
WorldViewColliderTool::on_primary_button_release (geom::fpoint const& pos)
{
  WorldGUIManager::instance()->ungrab_mouse(m_worldview);

  if (m_creating_rect)
  {
    geom::fpoint const pos2 = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());
    World& world = Controller::instance()->get_world();

    if (std::fabs(pos2.x() - m_click_pos.x()) < 15 ||
        std::fabs(pos2.y() - m_click_pos.y()) < 15)
    {
      log_debug("Rect collider to small, not inserting");
    }
    else
    {
      world.add_rect_collider(m_click_pos.as_vec(), pos2.as_vec());
    }
  }

  m_creating_rect = false;
  m_move_collider = nullptr;
}

void
WorldViewColliderTool::on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset)
{
  geom::fpoint current_pos = m_worldview.zoom().screen_to_world(pos);

  if (m_move_collider)
  {
    m_move_collider->set_pos(current_pos.as_vec() - m_click_pos.as_vec());
  }
}

void
WorldViewColliderTool::on_secondary_button_press (geom::fpoint const& pos)
{
  World const& world = Controller::instance()->get_world();

  m_to_delete_collider = world.find_collider(m_worldview.zoom().screen_to_world(pos));
}

void
WorldViewColliderTool::on_secondary_button_release (geom::fpoint const& pos)
{
  World& world = Controller::instance()->get_world();

  if (m_to_delete_collider == world.find_collider(m_worldview.zoom().screen_to_world(pos)))
  {
    Controller::instance()->push_undo();
    world.remove_collider(m_to_delete_collider);
  }
  m_to_delete_collider = nullptr;
}

} // namespace construo

/* EOF */
