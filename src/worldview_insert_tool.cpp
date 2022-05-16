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

#include "worldview_insert_tool.hpp"

#include <logmich/log.hpp>

#include "colors.hpp"
#include "particle_factory.hpp"
#include "input_context.hpp"
#include "graphic_context.hpp"
#include "world.hpp"
#include "controller.hpp"
#include "world_renderer.hpp"
#include "worldview_component.hpp"
#include "world_gui_manager.hpp"

WorldViewInsertTool::WorldViewInsertTool(WorldViewComponent& worldview) :
  WorldViewTool(worldview),
  m_previous_particle(nullptr),
  m_hover_spring(nullptr),
  m_hover_particle(nullptr)
{
}

WorldViewInsertTool::~WorldViewInsertTool()
{
}

void
WorldViewInsertTool::draw_background(ZoomGraphicContext& gc)
{
  if (m_hover_particle) {
    WorldRenderer::draw_particle_highlight(gc, *m_hover_particle);
  }
}

void
WorldViewInsertTool::draw_foreground(ZoomGraphicContext& gc)
{
  geom::fpoint click_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

  float const grid_size = m_worldview.get_snap_size();
  glm::vec2 new_particle_pos(0.0f, 0.0f);
  if (m_worldview.uses_grid()) {
    new_particle_pos = glm::vec2(Math::round_to_float(click_pos.x(), grid_size),
                                 Math::round_to_float(click_pos.y(), grid_size));
  } else {
    new_particle_pos = glm::vec2(click_pos.x(), click_pos.y());
  }

  if (m_hover_particle) {
    new_particle_pos = m_hover_particle->pos;
  }

  if (m_previous_particle)
  {
    gc.draw_line(m_previous_particle->pos, new_particle_pos,
                 g_style.new_spring, 2);
  }
  else
  {
    if (m_hover_particle) {
      WorldRenderer::draw_particle_info(gc, *m_hover_particle);
    } else {
      if (m_hover_spring) {
        WorldRenderer::draw_spring_highlight(gc, *m_hover_spring);
      }

      { // draw where new particle would be inserted
        gc.get_parent_gc().draw_fill_circle(gc.zoom().world_to_screen(new_particle_pos),
                                            std::max(3.0f,
                                                     gc.zoom().get_scale() *
                                                     WorldRenderer::mass_to_radius(Controller::instance()->get_particle_mass())),
                                            g_style.highlight);
      }
    }
  }
}

void
WorldViewInsertTool::on_mouse_move(geom::fpoint const& screen_pos, geom::foffset const& offset)
{
  World const& world = Controller::instance()->get_world();

  geom::fpoint const pos = m_worldview.zoom().screen_to_world(screen_pos);
  float const capture_distance = 20.0f / m_worldview.zoom().get_scale();

  m_hover_particle = world.find_particle(pos, capture_distance);
  m_hover_spring = world.find_spring(pos, capture_distance);
}

void
WorldViewInsertTool::on_primary_button_press(geom::fpoint const& screen_pos)
{
  World& world = Controller::instance()->get_world();
  float const x = m_worldview.zoom().screen_to_world_x(screen_pos.x());
  float const y = m_worldview.zoom().screen_to_world_y(screen_pos.y());

  if (m_previous_particle) // create a spring that connects two particles
  {
    // We are going to place a second particle and connect the last an
    // the new one with a spring
    if (m_hover_particle != m_previous_particle)
    {
      if (m_hover_particle) // connect to particles
      {
        world.add_spring(m_previous_particle, m_hover_particle);
      }
      else // add a new particle and connect it with the current one
      {
        glm::vec2 new_particle_pos(0.0f, 0.0f);

        float const grid_size = m_worldview.get_snap_size();
        if (m_worldview.uses_grid()) {
          new_particle_pos = glm::vec2(Math::round_to_float(x, grid_size),
                                       Math::round_to_float(y, grid_size));
        } else {
          new_particle_pos = glm::vec2(x, y);
        }

        if (new_particle_pos == m_previous_particle->pos) {
          log_debug("spring creation with length 0 requested, ignore");
        } else {
          Particle* const new_particle = world.get_particle_mgr().add_particle(new_particle_pos,
                                                                               glm::vec2(0.0f, 0.0f),
                                                                               Controller::instance()->get_particle_mass());
          world.add_spring(m_previous_particle, new_particle);
        }
      }
      // Lower the spring links count, since we have increased it
      // at insertion and now connected it to a real spring, so
      // its no longer needed
      m_previous_particle->spring_links -= 1;

      m_previous_particle = nullptr;
    }
    WorldGUIManager::instance()->ungrab_mouse(m_worldview);
  }
  else
  {
    // We are going to create a new particle and making it the
    // current one, so that the next click would result in a new
    // spring
    m_previous_particle = m_hover_particle;

    if (m_previous_particle)
    {
      // connect a spring to an existing particle
    }
    else
    {
      // insert a new particle
      float const grid_size = m_worldview.get_snap_size();

      glm::vec2 new_particle_pos(0.0f, 0.0f);
      if (m_worldview.uses_grid()) {
        new_particle_pos = glm::vec2(Math::round_to_float(x, grid_size),
                                     Math::round_to_float(y, grid_size));
      } else {
        new_particle_pos = glm::vec2(x, y);
      }

      Particle* p = world.get_particle_mgr().add_particle(new_particle_pos,
                                                          glm::vec2(0.0f, 0.0f),
                                                          Controller::instance()->get_particle_mass());
      m_previous_particle = p;
      // Increase the spring count so that the particle isn't cleaned up
      m_previous_particle->spring_links += 1;
    }

    WorldGUIManager::instance()->grab_mouse(m_worldview);
  }
}

void
WorldViewInsertTool::on_primary_button_release (geom::fpoint const& pos)
{
}

void
WorldViewInsertTool::on_secondary_button_press (geom::fpoint const& screen_pos)
{
  on_delete_press(screen_pos);
}

void
WorldViewInsertTool::on_secondary_button_release (geom::fpoint const& screen_pos)
{
}

void
WorldViewInsertTool::on_delete_press(geom::fpoint const& screen_pos)
{
  World& world = Controller::instance()->get_world ();

  if (m_previous_particle)
  { // We are currently creating a new spring, abort that
    m_previous_particle->spring_links -= 1;
    m_previous_particle = nullptr;
    m_hover_particle = nullptr;
    m_hover_spring = nullptr;
    WorldGUIManager::instance()->ungrab_mouse(m_worldview);
  }
  else
  {
    if (m_hover_particle)
    {
      Controller::instance()->push_undo();
      world.remove_particle(m_hover_particle);
      m_hover_particle = nullptr;
      m_hover_spring = nullptr;
    }
    else if (m_hover_spring)
    {
      Controller::instance()->push_undo();
      world.remove_spring(m_hover_spring);
      m_hover_particle = nullptr;
      m_hover_spring = nullptr;
    }
  }
}

void
WorldViewInsertTool::on_fix_press (geom::fpoint const& screen_pos)
{
  if (m_hover_particle) {
    m_hover_particle->set_fixed(!m_hover_particle->get_fixed());
  }
}

/* EOF */
