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

#include <float.h>
#include "colors.hpp"
#include "particle_factory.hpp"
#include "input_context.hpp"
#include "graphic_context.hpp"
#include "world.hpp"
#include "controller.hpp"
#include "worldview_component.hpp"
#include "worldview_insert_tool.hpp"
#include "world_gui_manager.hpp"

WorldViewInsertTool::WorldViewInsertTool(WorldViewComponent& worldview) :
  WorldViewTool(worldview),
  m_current_particle(nullptr),
  m_particle_mass(0.1f),
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
    m_hover_particle->draw_highlight(gc);
  }
}

void
WorldViewInsertTool::draw_foreground(ZoomGraphicContext& gc)
{
  Vector2d click_pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

  float const grid_size = m_worldview.get_snap_size();
  Vector2d new_particle_pos;
  if (m_worldview.uses_grid()) {
    new_particle_pos = Vector2d(Math::round_to_float(click_pos.x, grid_size),
                                Math::round_to_float(click_pos.y, grid_size));
  } else {
    new_particle_pos = Vector2d(click_pos.x, click_pos.y);
  }

  if (m_hover_particle) {
    new_particle_pos = m_hover_particle->pos;
  }

  if (m_current_particle)
  {
    gc.GraphicContext::draw_line(m_current_particle->pos, new_particle_pos,
                                 g_style.new_spring, 2);
  }
  else
  {
    if (m_hover_particle) {
      m_hover_particle->draw_infos(gc);
    } else {
      if (m_hover_spring) {
        m_hover_spring->draw_highlight (gc);
      }

      { // draw where new particle would be inserted
        gc.draw_fill_circle(new_particle_pos.x,
                            new_particle_pos.y,
                            3.0f / m_worldview.zoom().get_scale(),
                            g_style.highlight);
      }
    }
  }
}

void
WorldViewInsertTool::on_mouse_move(float x, float y, float of_x, float of_y)
{
  World const& world = *Controller::instance()->get_world();
  Vector2d const pos = m_worldview.zoom().screen_to_world(Vector2d(x, y));

  float const capture_distance = 20.0f / m_worldview.zoom().get_scale();

  m_hover_particle = world.get_particle(pos.x, pos.y, capture_distance);
  m_hover_spring = world.get_spring(pos.x, pos.y, capture_distance);
}

void
WorldViewInsertTool::on_primary_button_press(float screen_x, float screen_y)
{
  World& world = *Controller::instance()->get_world();
  float x = m_worldview.zoom().screen_to_world_x (screen_x);
  float y = m_worldview.zoom().screen_to_world_y (screen_y);

  if (m_current_particle)
  {
    // We are going to place a second particle and connect the last an
    // the new one with a spring
    Particle* new_current_particle = m_hover_particle;
    if (new_current_particle != m_current_particle)
    {
      if (new_current_particle) // connect to particles
      {
        world.add_spring (m_current_particle, new_current_particle);
      }
      else // add a new particle and connect it with the current one
      {
        Vector2d new_particle_pos;

        float grid_size = m_worldview.get_snap_size();

        if (m_worldview.uses_grid())
          new_particle_pos = Vector2d(Math::round_to_float(x, grid_size),
                                      Math::round_to_float(y, grid_size));
        else
          new_particle_pos = Vector2d(x, y);

        new_current_particle = world.get_particle_mgr().add_particle(new_particle_pos,
                                                                     Vector2d(),
                                                                     m_particle_mass);
        world.add_spring (m_current_particle, new_current_particle);
      }
      // Lower the spring links count, since we have increased it
      // at insertion and now connected it to a real spring, so
      // its no longer needed
      m_current_particle->spring_links -= 1;

      m_current_particle = nullptr;
    }
    WorldGUIManager::instance()->ungrab_mouse(m_worldview);
  }
  else
  {
    // We are going to create a new particle and making it the
    // current one, so that the next click would result in a new
    // spring
    m_current_particle = m_hover_particle;

    if (m_current_particle)
    {
      // connect a spring to an existing particle
    }
    else
    {
      // insert a new particle
      float const grid_size = m_worldview.get_snap_size();

      Vector2d new_particle_pos;
      if (m_worldview.uses_grid()) {
        new_particle_pos = Vector2d(Math::round_to_float(x, grid_size),
                                    Math::round_to_float(y, grid_size));
      } else {
        new_particle_pos = Vector2d(x, y);
      }

      Particle* p = world.get_particle_mgr().add_particle(new_particle_pos,
                                                          Vector2d(),
                                                          m_particle_mass);
      m_current_particle = p;
      // Increase the spring count so that the particle isn't cleaned up
      m_current_particle->spring_links += 1;
    }

    WorldGUIManager::instance()->grab_mouse(m_worldview);
  }
}

void
WorldViewInsertTool::on_primary_button_release (float x, float y)
{
}

void
WorldViewInsertTool::on_secondary_button_press (float screen_x, float screen_y)
{
  on_delete_press(screen_x, screen_y);
}

void
WorldViewInsertTool::on_secondary_button_release (float screen_x, float screen_y)
{
}

void
WorldViewInsertTool::on_delete_press(float screen_x, float screen_y)
{
  World& world = *Controller::instance()->get_world ();

  if (m_current_particle)
  { // We are currently creating a new spring, abort that
    m_current_particle->spring_links -= 1;
    m_current_particle = nullptr;
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
WorldViewInsertTool::on_fix_press (float screen_x, float screen_y)
{
  if (m_hover_particle) {
    m_hover_particle->set_fixed(!m_hover_particle->get_fixed());
  }
}

/* EOF */
