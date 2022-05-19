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

#include "worldview_component.hpp"

#include "colors.hpp"
#include "controller.hpp"
#include "root_graphic_context.hpp"
#include "world_gui_manager.hpp"
#include "world_renderer.hpp"
#include "worldview_tool.hpp"
#include "worldview_insert_tool.hpp"
#include "worldview_select_tool.hpp"
#include "worldview_zoom_tool.hpp"
#include "worldview_collider_tool.hpp"

WorldViewComponent* WorldViewComponent::instance_;

WorldViewComponent::WorldViewComponent() :
  GUIComponent(),
  m_zoom(g_graphic_context->geometry()),
  m_use_grid(false),
  m_grid_base_size(10),
  m_grid_constant(5),
  m_grid_scale_factor(1),
  m_grid_snap_factor(2),
  m_scrolling(false),
  m_scroll_pos(0.0f, 0.0f),
  m_offset(0.0f, 0.0f),
  m_select_tool(std::make_unique<WorldViewSelectTool>(*this)),
  m_insert_tool(std::make_unique<WorldViewInsertTool>(*this)),
  m_zoom_tool(std::make_unique<WorldViewZoomTool>(*this)),
  m_collider_tool(std::make_unique<WorldViewColliderTool>(*this)),
  m_current_tool(m_insert_tool.get()),
  m_mode(INSERT_MODE)
{
  instance_ = this;
}

void
WorldViewComponent::set_mode(Mode m)
{
  m_current_tool->deactivate();

  if (m == INSERT_MODE)
  {
    m_current_tool = m_insert_tool.get();
    m_mode = INSERT_MODE;
    g_graphic_context->set_cursor(CursorType::INSERT);
  }
  else if (m == SELECT_MODE)
  {
    m_current_tool = m_select_tool.get();
    m_mode = SELECT_MODE;
    g_graphic_context->set_cursor(CursorType::SELECT);
  }
  else if (m == ZOOM_MODE)
  {
    m_current_tool = m_zoom_tool.get();
    m_mode = ZOOM_MODE;
    g_graphic_context->set_cursor(CursorType::ZOOM);
  }
  else if (m == COLLIDER_MODE)
  {
    m_current_tool = m_collider_tool.get();
    m_mode = COLLIDER_MODE;
    g_graphic_context->set_cursor(CursorType::COLLIDER);
  }
  else
  {
    assert(false && "WorldViewComponent: unknown mode");
  }

  m_current_tool->activate ();
}

WorldViewComponent::~WorldViewComponent()
{
  instance_ = nullptr;
}

float
WorldViewComponent::get_grid_size()
{
  return m_grid_base_size / powf(static_cast<float>(m_grid_constant),
                                 static_cast<float>(Math::get_exp_n(m_zoom.get_scale() * m_grid_scale_factor, m_grid_constant)));
}

float
WorldViewComponent::get_snap_size()
{
  return get_grid_size() / m_grid_snap_factor;
}

void
WorldViewComponent::draw(GraphicContext& parent_gc)
{
  m_zoom.set_bounding_box(m_geometry);
  ZoomGraphicContext gc(parent_gc, m_zoom);

  World& world = Controller::instance()->get_world();
  WorldRenderer renderer(world);

  if (m_use_grid) {
    renderer.draw_grid(gc, get_grid_size(), m_grid_constant);
  }

  renderer.draw_ground(gc);

  if (Controller::instance()->get_action_cam()
      && Controller::instance()->is_running())
  {
    // Live Action Cam
    geom::frect const& box = world.calc_bounding_box();
    // Zoom to the bounding box
    m_zoom.zoom_to(box);
    // Zoom out two times so that the area isn't covered up by the GUI
    m_zoom.zoom_out(geom::center(geometry()));
    m_zoom.zoom_out(geom::center(geometry()));
  }

  m_current_tool->draw_background(gc);

  if (0) // draw bounding box
  {
    gc.draw_rect(world.calc_bounding_box(), Color(1.0f, 1.0f, 1.0f));
  }

  renderer.draw_colliders(gc);
  renderer.draw_springs(gc);
  if (!Controller::instance()->get_hide_dots()) {
    renderer.draw_particles(gc);
  }

  m_current_tool->draw_foreground(gc);

  if (0)
  {
    switch (m_mode)
    {
      case ZOOM_MODE:
        parent_gc.draw_string(geom::fpoint(10, parent_gc.geometry().height() - 15), "[  Zoom Mode  ]");
        break;
      case INSERT_MODE:
        parent_gc.draw_string(geom::fpoint(10, parent_gc.geometry().height() - 15), "[ Insert Mode ]");
        break;
      case SELECT_MODE:
        parent_gc.draw_string(geom::fpoint(10, parent_gc.geometry().height() - 15), "[ Select Mode ]");
        break;
      case COLLIDER_MODE:
        parent_gc.draw_string(geom::fpoint(10, parent_gc.geometry().height() - 15), "[Collider Mode]");
        break;
    }
  }

  //const WorldBoundingBox& box = world.calc_bounding_box();
  //gc.flip(int(box.x1), int(box.y1), int(box.x2), int(box.y2));
}

void
WorldViewComponent::wheel_up(geom::fpoint const& pos)
{
  m_zoom.zoom_in(pos);
}

void
WorldViewComponent::wheel_down(geom::fpoint const& pos)
{
  m_zoom.zoom_out(pos);
}

void
WorldViewComponent::on_button_press(int button_id, geom::fpoint const& pos)
{
  m_current_tool->on_button_press(button_id, pos);
}

void
WorldViewComponent::on_primary_button_press(geom::fpoint const& screen_pos)
{
  m_current_tool->on_primary_button_press(screen_pos);
}

void
WorldViewComponent::on_primary_button_release(geom::fpoint const& screen_pos)
{
  m_current_tool->on_primary_button_release(screen_pos);
}

void
WorldViewComponent::on_secondary_button_press(geom::fpoint const& screen_pos)
{
  m_current_tool->on_secondary_button_press(screen_pos);
}

void
WorldViewComponent::on_secondary_button_release(geom::fpoint const& screen_pos)
{
  m_current_tool->on_secondary_button_release(screen_pos);
}

void
WorldViewComponent::on_delete_press(geom::fpoint const& screen_pos)
{
  m_current_tool->on_delete_press(screen_pos);
}

void
WorldViewComponent::on_duplicate_press(geom::fpoint const& screen_pos)
{
  m_current_tool->on_duplicate_press(screen_pos);
}

void
WorldViewComponent::on_join_press(geom::fpoint const& pos)
{
  m_current_tool->on_join_press(pos);
}

void
WorldViewComponent::on_fix_press(geom::fpoint const& screen_pos)
{
  m_current_tool->on_fix_press(screen_pos);
}

void
WorldViewComponent::scroll_left()
{
  m_zoom.translate(geom::foffset(-20, 0));
}

void
WorldViewComponent::scroll_right()
{
  m_zoom.translate(geom::foffset(20, 0));
}

void
WorldViewComponent::scroll_up()
{
  m_zoom.translate(geom::foffset(0, -20));
}

void
WorldViewComponent::scroll_down()
{
  m_zoom.translate(geom::foffset(0, 20));
}

void
WorldViewComponent::on_tertiary_button_press(geom::fpoint const& pos)
{
  m_scrolling = true;
  g_graphic_context->push_cursor();
  g_graphic_context->set_cursor(CursorType::SCROLL);

  m_offset = m_zoom.get_offset();

  WorldGUIManager::instance()->grab_mouse(*this);

  m_scroll_pos = m_zoom.screen_to_world(pos);
}

void
WorldViewComponent::on_tertiary_button_release(geom::fpoint const& pos)
{
  g_graphic_context->pop_cursor();
  m_scrolling = false;
  WorldGUIManager::instance()->ungrab_mouse(*this);
}

void
WorldViewComponent::on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset)
{
  if (m_scrolling)
  {
    geom::fpoint const new_scroll_pos(pos.x() / m_zoom.get_scale() - m_offset.x(),
                                      pos.y() / m_zoom.get_scale() - m_offset.y());

    m_zoom.set_offset(geom::foffset(m_offset.x() + (new_scroll_pos.x() - m_scroll_pos.x()),
                                    m_offset.y() + (new_scroll_pos.y() - m_scroll_pos.y())));

  }
  else
  {
    m_current_tool->on_mouse_move(pos, offset);
  }
}

void
WorldViewComponent::on_scale_press(geom::fpoint const& pos)
{
  m_current_tool->on_scale_press(pos);
}

void
WorldViewComponent::on_grid_press(geom::fpoint const& pos)
{
  m_use_grid = !m_use_grid;
}

float
WorldViewComponent::get_scale ()
{
  return m_zoom.get_scale();
}

void
WorldViewComponent::on_world_change()
{
  World& world = Controller::instance()->get_world();

  geom::frect const& box = world.calc_bounding_box();
  // Zoom to the bounding box
  m_zoom.zoom_to(box);
  // Zoom out two times so that the area isn't covered up by the
  // GUI
  m_zoom.zoom_out(geom::fpoint(geometry().width()/2, geometry().height()/2));
  m_zoom.zoom_out(geom::fpoint(geometry().width()/2, geometry().height()/2));
}

/* EOF */
