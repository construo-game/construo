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

#include <cmath>

#include <geom/rect.hpp>

#include "colors.hpp"
#include "controller.hpp"
#include "math.hpp"
#include "particle.hpp"
#include "root_graphic_context.hpp"
#include "world.hpp"
#include "world_renderer.hpp"
#include "world_gui_manager.hpp"
#include "worldview_widget.hpp"
#include "worldview_select_tool.hpp"

namespace construo {

WorldViewSelectTool::WorldViewSelectTool(WorldViewWidget& worldview) :
  WorldViewTool(worldview),
  m_selection(),
  m_mode(IDLE_MODE),
  m_click_pos(0.0f, 0.0f),
  m_move_diff(0.0f, 0.0f),
  m_move_current_particle(nullptr),
  m_rotate_center(0.0f, 0.0f),
  m_scale_center(0.0f, 0.0f),
  m_old_scale_factor(1.0f)
{
}

WorldViewSelectTool::~WorldViewSelectTool ()
{
}

void
WorldViewSelectTool::draw_background (ZoomGraphicContext& gc)
{
  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    WorldRenderer::draw_particle_velocity_vector(gc, **i);
    WorldRenderer::draw_particle_highlight(gc, **i);
  }
}

void
WorldViewSelectTool::draw_foreground (ZoomGraphicContext& gc)
{
  geom::fpoint const pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

  if (m_mode == GETTING_SELECTION_MODE)
  {
    gc.draw_rect(geom::normalize(geom::frect(pos, m_click_pos)), g_style.selection_rect);
  }

  if (!m_selection.empty())
  {
    Particle& p = **m_selection.begin();
    geom::frect selection_box(p.pos, p.pos);

    for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
    {
      selection_box = geom::frect(std::min(selection_box.left(), (*i)->pos.x),
                                  std::min(selection_box.top(), (*i)->pos.y),
                                  std::max(selection_box.right(), (*i)->pos.x),
                                  std::max(selection_box.bottom(), (*i)->pos.y));
    }

    float border = 20.0f / gc.zoom().get_scale();
    gc.draw_rect(geom::grow(selection_box, border), g_style.new_spring);

    if (0) // draw selection rect
    {
      float rsize = 5.0f / gc.zoom().get_scale();
      gc.draw_fill_rect(geom::frect(selection_box.left() - border - rsize, selection_box.top() - border - rsize,
                                    selection_box.left() - border + rsize, selection_box.top() - border + rsize),
                        g_style.selection_resizer);
      gc.draw_fill_rect(geom::frect(selection_box.right() + border - rsize, selection_box.top() - border - rsize,
                                    selection_box.right() + border + rsize, selection_box.top() - border + rsize),
                        g_style.selection_resizer);
      gc.draw_fill_rect(geom::frect(selection_box.left() - border - rsize, selection_box.bottom() + border - rsize,
                                    selection_box.left() - border + rsize, selection_box.bottom() + border + rsize),
                        g_style.selection_resizer);
      gc.draw_fill_rect(geom::frect(selection_box.right() + border - rsize, selection_box.bottom() + border - rsize,
                                    selection_box.right() + border + rsize, selection_box.bottom() + border + rsize),
                        g_style.selection_resizer);
    }

    gc.get_parent_gc().draw_circle(gc.zoom().world_to_screen(m_selection.get_center ()),
                                   8.0f, g_style.selection_rect);
    gc.get_parent_gc().draw_circle(gc.zoom().world_to_screen(m_selection.get_center ()),
                                   16.0f, g_style.selection_rect);
  }
}

void
WorldViewSelectTool::deactivate ()
{
  m_selection.clear ();
}

void
WorldViewSelectTool::on_primary_button_press(geom::fpoint const& screen_pos)
{
  switch (m_mode)
  {
    case IDLE_MODE:
      {
        geom::fpoint pos = m_worldview.zoom().screen_to_world(screen_pos);

        World& world = Controller::instance()->get_world();

        WorldGUIManager::instance()->grab_mouse(m_worldview);

        m_mode = GETTING_SELECTION_MODE;

        m_click_pos = pos;

        float const capture_distance = 20.0f / m_worldview.zoom().get_scale();

        // If the mouse clicks on a particle from the selection, we move the selection
        Particle* new_current_particle = world.find_particle(pos, capture_distance);
        for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
        {
          if (new_current_particle == *i)
          {
            Controller::instance()->push_undo();
            m_mode = MOVING_SELECTION_MODE;
            m_move_diff = glm::vec2(0.0f, 0.0f);
            m_move_current_particle = new_current_particle;
            break;
          }
        }

        // If mouse clicks into empty space, we make a new selection
        if (m_mode == GETTING_SELECTION_MODE)
        {
          m_selection.clear();
        }
      }
      break;

    case SCALING_SELECTION_MODE:
      {
        g_graphic_context->pop_cursor();
        WorldGUIManager::instance()->ungrab_mouse(m_worldview);
        m_mode = IDLE_MODE;
        g_graphic_context->pop_cursor();
        m_old_scale_factor = 1.0f;
      }

    default:
      // Do nothing, so that we don't mess up other modes
      break;
  }
}

void
WorldViewSelectTool::on_primary_button_release (geom::fpoint const& pos)
{
  WorldGUIManager::instance()->ungrab_mouse(m_worldview);

  switch(m_mode)
  {
    case GETTING_SELECTION_MODE:
      {
        m_selection.select_particles(m_click_pos, m_worldview.zoom().screen_to_world(pos));
        m_mode = IDLE_MODE;
      }
      break;

    case MOVING_SELECTION_MODE:
      {
        m_mode = IDLE_MODE;
      }
      break;

    case SCALING_SELECTION_MODE:
      break;

    case ROTATING_SELECTION_MODE:
      break;

    case IDLE_MODE:
      break;
  }
}

void
WorldViewSelectTool::on_secondary_button_press (geom::fpoint const& screen_pos)
{
  switch (m_mode)
  {
    case IDLE_MODE:
      if (!m_selection.empty())
      {
        Controller::instance()->push_undo();
        g_graphic_context->push_cursor();
        g_graphic_context->set_cursor(CursorType::ROTATE);

        m_mode = ROTATING_SELECTION_MODE;
        WorldGUIManager::instance()->grab_mouse(m_worldview);

        m_click_pos = m_worldview.zoom().screen_to_world(screen_pos);

        m_rotate_center = m_selection.get_center();
      }
      break;

    case SCALING_SELECTION_MODE:
      {
        g_graphic_context->pop_cursor();
        WorldGUIManager::instance()->ungrab_mouse(m_worldview);
        m_mode = IDLE_MODE;
        g_graphic_context->pop_cursor();
        m_selection.scale(1.0f / m_old_scale_factor, m_scale_center);
        m_old_scale_factor = 1.0f;
      }
      break;

    default:
      break;
  }
}

void
WorldViewSelectTool::on_secondary_button_release (geom::fpoint const& pos)
{
  switch (m_mode)
  {
    case ROTATING_SELECTION_MODE:
      g_graphic_context->pop_cursor();
      WorldGUIManager::instance()->ungrab_mouse(m_worldview);
      m_mode = IDLE_MODE;
      break;

    default:
      break;
  }
}

void
WorldViewSelectTool::on_delete_press (geom::fpoint const& pos)
{
  Controller::instance()->push_undo();

  World& world = Controller::instance()->get_world();
  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    world.remove_particle(*i);
  }

  m_selection.clear();
}

void
WorldViewSelectTool::on_fix_press(geom::fpoint const& pos)
{
  bool mark_all = false;
  for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
  {
    if (!(*i)->get_fixed())
    {
      mark_all = true;
    }
  }

  if (mark_all)
  {
    for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
    {
      (*i)->set_fixed(true);
    }
  }
  else
  {
    for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
    {
      (*i)->set_fixed(!(*i)->get_fixed());
    }
  }
}

void
WorldViewSelectTool::on_mouse_move(geom::fpoint const& screen_pos, geom::foffset const& offset)
{
  World& world = Controller::instance()->get_world();

  switch (m_mode)
  {
    case MOVING_SELECTION_MODE:
      {
        glm::vec2 const new_pos(m_worldview.zoom().screen_to_world(screen_pos).as_vec());

        glm::vec2 diff = new_pos - m_click_pos.as_vec();

        // Undo the last move (FIXME: Potential round errors)
        for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
        {
          (*i)->pos -= m_move_diff;
        }

        if (m_worldview.uses_grid())
        {
          float const grid_size = m_worldview.get_snap_size();

          diff.x = Math::round_to_float(diff.x, grid_size);
          diff.y = Math::round_to_float(diff.y, grid_size);
          diff.x -= m_move_current_particle->pos.x - Math::round_to_float(m_move_current_particle->pos.x, grid_size);
          diff.y -= m_move_current_particle->pos.y - Math::round_to_float(m_move_current_particle->pos.y, grid_size);
        }

        m_move_diff = diff;

        for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
        {
          (*i)->pos += diff;

          // Recalculate all springs that are attached to the
          // selection, but not fully in it.
          for (auto& spring : world.springs())
          {
            if (spring->particles.first == *i || spring->particles.second == *i)
            {
              spring->recalc_length();
            }
          }
        }
      }
      break;

    case SCALING_SELECTION_MODE:
      {
        geom::fpoint const new_pos = m_worldview.zoom().screen_to_world(screen_pos);
        float const scale_factor = std::fabs(glm::length(m_scale_center - new_pos.as_vec()) / glm::distance(m_scale_center, m_click_pos.as_vec()));
        m_selection.scale(1.0f / m_old_scale_factor, m_scale_center);
        m_selection.scale(scale_factor, m_scale_center);
        m_old_scale_factor = scale_factor;
      }
      break;

    case ROTATING_SELECTION_MODE:
      {
        glm::vec2 new_pos = m_worldview.zoom().screen_to_world(screen_pos).as_vec();

        float new_angle = atan2(new_pos.y - m_rotate_center.y,
                                new_pos.x - m_rotate_center.x);
        float old_angle = atan2(m_click_pos.y() - m_rotate_center.y,
                                m_click_pos.x() - m_rotate_center.x);
        float rot_angle = new_angle - old_angle;

        m_selection.rotate(rot_angle, m_rotate_center);

        m_click_pos = new_pos;
      }
      break;
    default:
      break;
  }
}

void
WorldViewSelectTool::on_scale_press(geom::fpoint const& pos)
{
  if (!m_selection.empty())
  {
    m_rotate_center = m_selection.get_center();
    Controller::instance()->push_undo();
    g_graphic_context->push_cursor();
    g_graphic_context->set_cursor(CursorType::SCALE);

    m_click_pos = m_worldview.zoom().screen_to_world(pos);
    WorldGUIManager::instance()->grab_mouse(m_worldview);

    m_mode = SCALING_SELECTION_MODE;

    m_scale_center = m_selection.get_center();
  }
}

void
WorldViewSelectTool::on_duplicate_press(geom::fpoint const& pos)
{
  m_selection.duplicate();
}

void
WorldViewSelectTool::on_button_press(int button_id, geom::fpoint const& screen_pos)
{
  geom::fpoint pos = m_worldview.zoom().screen_to_world(screen_pos);

  switch (static_cast<Action>(button_id))
  {
    case Action::SETVELOCITY:
      if (!m_selection.empty()) {
        m_selection.set_velocity(pos.as_vec() - m_selection.get_center());
      }
      break;

    case Action::FLIP:
      m_selection.flip();
      break;

    default:
      break;
  }
}

void
WorldViewSelectTool::on_join_press(geom::fpoint const& pos)
{
  m_selection.join_doubles(5.0f);
}

} // namespace construo

/* EOF */
