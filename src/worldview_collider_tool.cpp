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

#include "construo.hpp"
#include "input_context.hpp"
#include "controller.hpp"
#include "colors.hpp"
#include "world.hpp"
#include "world_gui_manager.hpp"
#include "worldview_component.hpp"
#include "worldview_collider_tool.hpp"

WorldViewColliderTool::WorldViewColliderTool(WorldViewComponent& worldview) :
  WorldViewTool(worldview),
  m_creating_rect(false),
  m_click_pos(),
  m_to_delete_collider(nullptr),
  m_move_collider(nullptr)
{
}

WorldViewColliderTool::~WorldViewColliderTool()
{
}

void
WorldViewColliderTool::draw_background (ZoomGraphicContext& gc)
{
  Vector2d mouse_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos ());
  if (m_creating_rect)
    {
      gc.GraphicContext::draw_rect(m_click_pos, mouse_pos, g_style.selection_rect);
    }
}

Collider*
WorldViewColliderTool::get_collider (const Vector2d& pos)
{
  World& world = *Controller::instance()->get_world();
  World::Colliders& colliders = world.get_colliders();
  for (World::Colliders::reverse_iterator i = colliders.rbegin ();
       i != colliders.rend(); ++i)
    {
      if ((*i)->is_at(pos))
        return *i;
    }
  return nullptr;
}

void
WorldViewColliderTool::draw_foreground (ZoomGraphicContext& gc)
{
  Vector2d mouse_pos
    = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos ());
  Collider* collider = get_collider (mouse_pos);

  if (collider)
    collider->draw_highlight(gc);
}

void
WorldViewColliderTool::on_primary_button_press (float x, float y)
{
  WorldGUIManager::instance()->grab_mouse(m_worldview);

  m_click_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos ());

  if ((m_move_collider = get_collider (m_click_pos)) != nullptr)
    {
      // click_pos Offset, not position
      m_click_pos = m_click_pos - m_move_collider->get_pos();
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
WorldViewColliderTool::on_primary_button_release (float x, float y)
{
  WorldGUIManager::instance()->ungrab_mouse(m_worldview);

  if (m_creating_rect)
    {
      Vector2d pos2 = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos ());
      World& world = *Controller::instance()->get_world();

      if (fabs(pos2.x - m_click_pos.x) < 15
          || fabs(pos2.y - m_click_pos.y) < 15)
        {
          std::cout << "Rect collider to small, not inserting" << std::endl;
        }
      else
        {
          world.add_rect_collider(m_click_pos, pos2);
        }
    }

  m_creating_rect = false;
  m_move_collider = nullptr;
}

void
WorldViewColliderTool::on_mouse_move (float x, float y, float of_x, float of_y)
{
  Vector2d current_pos = m_worldview.zoom().screen_to_world(Vector2d(x, y));

  if (m_move_collider)
  {
    m_move_collider->set_pos(current_pos - m_click_pos);
  }
}

void
WorldViewColliderTool::on_secondary_button_press (float x, float y)
{
  m_to_delete_collider = get_collider(m_worldview.zoom().screen_to_world(Vector2d(x, y)));
}

void
WorldViewColliderTool::on_secondary_button_release (float x, float y)
{
  World& world = *Controller::instance()->get_world();

  if (m_to_delete_collider
      == get_collider(m_worldview.zoom().screen_to_world(Vector2d(x, y))))
    {
      Controller::instance()->push_undo();
      world.remove_collider(m_to_delete_collider);
    }
  m_to_delete_collider = nullptr;
}

/* EOF */
