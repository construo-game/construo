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

#include <math.h>
#include "math.hpp"
#include "world.hpp"
#include "colors.hpp"
#include "controller.hpp"
#include "worldview_component.hpp"
#include "particle.hpp"
#include "world_gui_manager.hpp"
#include "rect.hpp"
#include "root_graphic_context.hpp"
#include "worldview_select_tool.hpp"

WorldViewSelectTool::WorldViewSelectTool () :
  m_selection(),
  m_mode(IDLE_MODE),
  m_click_pos(),
  m_move_diff(),
  m_move_current_particle(nullptr),
  m_rotate_center(),
  m_scale_center(),
  m_old_scale_factor(1.0f)
{
}

WorldViewSelectTool::~WorldViewSelectTool ()
{
}

void
WorldViewSelectTool::draw_background (ZoomGraphicContext* gc)
{
  for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
    {
      (*i)->draw_velocity_vector (gc);
      (*i)->draw_highlight (gc);
    }
}

void
WorldViewSelectTool::draw_foreground (ZoomGraphicContext* gc)
{
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x(g_input_context->get_mouse_x ());
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y(g_input_context->get_mouse_y ());

  if (m_mode == GETTING_SELECTION_MODE)
    {
      gc->draw_rect (Math::min(x, m_click_pos.x),
                     Math::min(y, m_click_pos.y),
                     Math::max(x, m_click_pos.x),
                     Math::max(y, m_click_pos.y),
                     Colors::selection_rect);
    }

  if (!m_selection.empty())
    {
      Particle& p = **m_selection.begin();
      Rect<float> selection_box (p.pos.x, p.pos.y, p.pos.x, p.pos.y);

      for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
        {
          selection_box.x1 = Math::min(selection_box.x1, (*i)->pos.x);
          selection_box.y1 = Math::min(selection_box.y1, (*i)->pos.y);

          selection_box.x2 = Math::max(selection_box.x2, (*i)->pos.x);
          selection_box.y2 = Math::max(selection_box.y2, (*i)->pos.y);
        }

      float border = 20.0f / gc->get_zoom();
      gc->draw_rect (selection_box.x1 - border, selection_box.y1 - border,
                     selection_box.x2 + border, selection_box.y2 + border,
                     Colors::new_spring);

      if (0) // draw selection rect
        {
          float rsize = 5.0f / gc->get_zoom();
          gc->draw_fill_rect (selection_box.x1 - border - rsize, selection_box.y1 - border - rsize,
                              selection_box.x1 - border + rsize, selection_box.y1 - border + rsize,
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x2 + border - rsize, selection_box.y1 - border - rsize,
                              selection_box.x2 + border + rsize, selection_box.y1 - border + rsize,
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x1 - border - rsize, selection_box.y2 + border - rsize,
                              selection_box.x1 - border + rsize, selection_box.y2 + border + rsize,
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x2 + border - rsize, selection_box.y2 + border - rsize,
                              selection_box.x2 + border + rsize, selection_box.y2 + border + rsize,
                              Colors::selection_resizer);
        }

      gc->get_parent_gc()->draw_circle(gc->world_to_screen(m_selection.get_center ()),
                                       8.0f, Colors::selection_rect);
      gc->get_parent_gc()->draw_circle(gc->world_to_screen(m_selection.get_center ()),
                                       16.0f, Colors::selection_rect);
    }
}

void
WorldViewSelectTool::activate ()
{
}

void
WorldViewSelectTool::deactivate ()
{
  m_selection.clear ();
}

void
WorldViewSelectTool::on_primary_button_press (float screen_x, float screen_y)
{
  switch (m_mode)
    {
    case IDLE_MODE:
      {
        float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x);
        float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y);

        World& world = *Controller::instance()->get_world ();

        WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());

        m_mode = GETTING_SELECTION_MODE;

        m_click_pos.x = x;
        m_click_pos.y = y;

        // If the mouse clicks on a particle from the selection, we move the selection
        Particle* new_current_particle = world.get_particle (x, y);
        for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
          {
            if (new_current_particle == *i)
              {
                Controller::instance()->push_undo();
                m_mode = MOVING_SELECTION_MODE;
                m_move_diff = Vector2d();
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
        WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
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
WorldViewSelectTool::on_primary_button_release (float x, float y)
{
  WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());

  switch(m_mode)
  {
    case GETTING_SELECTION_MODE:
      {
        m_selection.select_particles(m_click_pos,
                                   WorldViewComponent::instance()->get_gc()->screen_to_world (Vector2d(x,y)));
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
WorldViewSelectTool::on_secondary_button_press (float screen_x, float screen_y)
{
  switch (m_mode)
    {
    case IDLE_MODE:
      if (!m_selection.empty())
        {
          Controller::instance()->push_undo();
          g_graphic_context->push_cursor();
          g_graphic_context->set_cursor(CURSOR_ROTATE);

          m_mode = ROTATING_SELECTION_MODE;
          WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());

          m_click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(screen_x, screen_y));

          m_rotate_center = m_selection.get_center();
        }
      break;

    case SCALING_SELECTION_MODE:
      {
        g_graphic_context->pop_cursor();
        WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
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
WorldViewSelectTool::on_secondary_button_release (float x, float y)
{
  switch (m_mode)
    {
    case ROTATING_SELECTION_MODE:
      g_graphic_context->pop_cursor();
      WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
      m_mode = IDLE_MODE;
      break;

    default:
      break;
    }
}

void
WorldViewSelectTool::on_delete_press (float x, float y)
{
  Controller::instance()->push_undo();

  World& world = *Controller::instance()->get_world ();
  for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
    {
      world.remove_particle(*i);
    }

  m_selection.clear ();
}

void
WorldViewSelectTool::on_fix_press (float x, float y)
{
  bool mark_all = false;
  for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
    {
      if (!(*i)->get_fixed())
        {
          mark_all = true;
        }
    }

  if (mark_all)
    {
      for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
        {
          (*i)->set_fixed (true);
        }
    }
  else
    {
      for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
        {
          (*i)->set_fixed (!(*i)->get_fixed());
        }
    }
}

void
WorldViewSelectTool::on_mouse_move (float screen_x, float screen_y, float of_x, float of_y)
{
  World& world = *Controller::instance()->get_world ();

  switch (m_mode)
    {
    case MOVING_SELECTION_MODE:
      {
        Vector2d new_pos(WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x),
                         WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y));

        Vector2d diff = new_pos - m_click_pos;

        // Undo the last move (FIXME: Potential round errors)
        for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
          {
            (*i)->pos -= m_move_diff;
          }

        if (WorldViewComponent::instance()->uses_grid())
          {

            float grid_size = WorldViewComponent::instance()->get_snap_size();

            diff.x = Math::round_to_float(diff.x, grid_size);
            diff.y = Math::round_to_float(diff.y, grid_size);
	    diff.x -= m_move_current_particle->pos.x - Math::round_to_float(m_move_current_particle->pos.x, grid_size);
	    diff.y -= m_move_current_particle->pos.y - Math::round_to_float(m_move_current_particle->pos.y, grid_size);
          }

        m_move_diff = diff;

        for (Selection::iterator i = m_selection.begin (); i != m_selection.end (); ++i)
          {
            (*i)->pos += diff;

            // Recalculate all springs that are attached to the
            // selection, but not fully in it.
            std::vector<Spring*>& spring_mgr = world.get_spring_mgr();
            for (std::vector<Spring*>::iterator j = spring_mgr.begin ();
                 j != spring_mgr.end (); ++j)
              {
                if ((*j)->particles.first == *i
                    || (*j)->particles.second == *i)
                  {
                    (*j)->recalc_length ();
                  }
              }
          }
      }
      break;

    case SCALING_SELECTION_MODE:
      {
        Vector2d const new_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(screen_x, screen_y));
        float const scale_factor = fabsf((m_scale_center - new_pos).norm() / (m_scale_center - m_click_pos).norm());
        m_selection.scale(1.0f / m_old_scale_factor, m_scale_center);
        m_selection.scale(scale_factor, m_scale_center);
        m_old_scale_factor = scale_factor;
      }
      break;
    case ROTATING_SELECTION_MODE:
      {
        Vector2d new_pos(WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x),
                         WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y));

        float new_angle = atan2(new_pos.y - m_rotate_center.y,
                                new_pos.x - m_rotate_center.x);
        float old_angle = atan2(m_click_pos.y - m_rotate_center.y,
                                m_click_pos.x - m_rotate_center.x);
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
WorldViewSelectTool::on_scale_press (float x, float y)
{
  if (!m_selection.empty())
    {
      m_rotate_center = m_selection.get_center();
      Controller::instance()->push_undo();
      g_graphic_context->push_cursor();
      g_graphic_context->set_cursor(CURSOR_SCALE);

      m_click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(static_cast<float>(x), static_cast<float>(y)));
      WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());

      m_mode = SCALING_SELECTION_MODE;

      m_scale_center = m_selection.get_center();
    }
}

void
WorldViewSelectTool::on_duplicate_press (float x, float y)
{
  m_selection.duplicate ();
}

void
WorldViewSelectTool::on_button_press (int button_id, float x, float y)
{
  Vector2d pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(static_cast<float>(x), static_cast<float>(y)));

  switch (button_id)
    {
    case BUTTON_SETVELOCITY:
      m_selection.set_velocity (pos - m_selection.get_center());
      break;

    case BUTTON_FLIP:
      m_selection.flip();
      break;

    default:
      break;
    }
}

void
WorldViewSelectTool::on_join_press (float x, float y)
{
  std::cout << "Join pressed" << std::endl;
  m_selection.join_doubles(5.0f);
}

/* EOF */
