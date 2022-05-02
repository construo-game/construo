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

#include "string_utils.hpp"
#include "construo.hpp"
#include "graphic_context.hpp"
#include "input_context.hpp"
#include "system_context.hpp"
#include "controller.hpp"
#include "events.hpp"
#include "gui_component.hpp"
#include "gui_manager.hpp"
#include "worldview_component.hpp"
#include "worldview_insert_tool.hpp"
#include "root_graphic_context.hpp"
#include "screen_manager.hpp"

using namespace StringUtils;

GUIManager::GUIManager() :
  m_frame_count(0),
  m_start_time(g_system_context->get_time()),
  m_current_fps(0.0f),
  m_last_component(nullptr),
  m_current_component(nullptr),
  m_grabbing_component(nullptr),
  m_last_x(),
  m_last_y(),
  m_components()
{
}

GUIManager::~GUIManager ()
{
}

void
GUIManager::run_once ()
{
  m_frame_count += 1;

  if (m_start_time + 3000 < g_system_context->get_time ())
    {
      float passed_time = (g_system_context->get_time () - m_start_time) / 1000.0f;

      //std::cout << "FPS: " << frame_count / passed_time << std::endl;

      m_current_fps = m_frame_count / passed_time;

      m_frame_count = 0;
      m_start_time  = g_system_context->get_time ();
    }

  process_events ();

  update();

  g_graphic_context->clear ();
  draw ();
  draw_overlay ();
  g_graphic_context->flip ();
}

void
GUIManager::draw ()
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
    {
      (*i)->draw (g_graphic_context);
    }
}

GUIComponent*
GUIManager::find_component_at(float x, float y)
{
  GUIComponent* component = nullptr;
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
    {
      if ((*i)->is_at(x, y))
        component = i->get();
    }
  return component;
}

void
GUIManager::process_button_events (ButtonEvent& button)
{
  float const x = g_input_context->get_mouse_x();
  float const y = g_input_context->get_mouse_y();

  if (button.pressed)
    {
      switch (button.id)
        {
        case BUTTON_FULLSCREEN:
          // graphic_centext->toggle_fullscreen();
          break;

        case BUTTON_START:
          Controller::instance()->start_simulation ();
          break;

        case BUTTON_PRIMARY:
          m_current_component->on_primary_button_press(x, y);
          break;

        case BUTTON_SECONDARY:
          m_current_component->on_secondary_button_press(x, y);
          break;

        case BUTTON_TERTIARY:
          m_current_component->on_tertiary_button_press(x, y);
          break;

        case BUTTON_SCALE:
          m_current_component->on_scale_press (x, y);
          break;

        case BUTTON_FIX:
          m_current_component->on_fix_press (x, y);
          break;

        case BUTTON_JOIN:
          m_current_component->on_join_press (x, y);
          break;

        case BUTTON_GRID:
          m_current_component->on_grid_press(x, y);
          break;

        case BUTTON_DELETE:
          m_current_component->on_delete_press (x, y);
          break;
        case BUTTON_DUPLICATE:
          m_current_component->on_duplicate_press (x, y);
          break;

        case BUTTON_SCROLL_LEFT:
          m_current_component->scroll_left ();
          break;

        case BUTTON_SCROLL_RIGHT:
          m_current_component->scroll_right ();
          break;

        case BUTTON_SCROLL_UP:
          m_current_component->scroll_up ();
          break;

        case BUTTON_SCROLL_DOWN:
          m_current_component->scroll_down ();
          break;

        case BUTTON_CLEAR:
          Controller::instance()->clear_world ();
          break;

        case BUTTON_UNDO:
          Controller::instance()->undo ();
          break;

        case BUTTON_REDO:
          Controller::instance()->redo ();
          break;

        case BUTTON_ACTIONCAM:
          Controller::instance()->set_action_cam (!Controller::instance()->get_action_cam ());
          break;

        case BUTTON_HIDEDOTS:
          Controller::instance()->set_hide_dots (!Controller::instance()->get_hide_dots ());
          break;

        case BUTTON_ESCAPE:
          ScreenManager::instance()->quit();
          break;

        case BUTTON_MODE_CHANGE:
          if (WorldViewComponent::instance()->get_mode () == WorldViewComponent::INSERT_MODE)
            {
              WorldViewComponent::instance()->set_mode(WorldViewComponent::SELECT_MODE);
            }
          else
            {
              WorldViewComponent::instance()->set_mode(WorldViewComponent::INSERT_MODE);
            }
          break;

        case BUTTON_TOGGLESLOWMO:
          Controller::instance()->set_slow_down (!Controller::instance()->slow_down_active ());
          break;

        case BUTTON_RUN:
          Controller::instance()->start_simulation ();
          break;

        case BUTTON_QUICKSAVE0:
        case BUTTON_QUICKSAVE1:
        case BUTTON_QUICKSAVE2:
        case BUTTON_QUICKSAVE3:
        case BUTTON_QUICKSAVE4:
        case BUTTON_QUICKSAVE5:
        case BUTTON_QUICKSAVE6:
        case BUTTON_QUICKSAVE7:
        case BUTTON_QUICKSAVE8:
        case BUTTON_QUICKSAVE9:
          Controller::instance()->save_to_slot (button.id - BUTTON_QUICKSAVE0);
          break;

        case BUTTON_QUICKLOAD0:
        case BUTTON_QUICKLOAD1:
        case BUTTON_QUICKLOAD2:
        case BUTTON_QUICKLOAD3:
        case BUTTON_QUICKLOAD4:
        case BUTTON_QUICKLOAD5:
        case BUTTON_QUICKLOAD6:
        case BUTTON_QUICKLOAD7:
        case BUTTON_QUICKLOAD8:
        case BUTTON_QUICKLOAD9:
          Controller::instance()->load_from_slot (button.id - BUTTON_QUICKLOAD0);
          break;

        case BUTTON_ZOOM_OUT:
          m_current_component->wheel_down (x, y);
          break;

        case BUTTON_ZOOM_IN:
          m_current_component->wheel_up (x, y);
          break;

        default:
          m_current_component->on_button_press (button.id, x, y);
          break;
        }
    }
  else // button released
    {
      switch (button.id)
        {
        case BUTTON_PRIMARY:
          m_current_component->on_primary_button_release(x, y);
          break;

        case BUTTON_SECONDARY:
          m_current_component->on_secondary_button_release(x, y);
          break;

        case BUTTON_TERTIARY:
          m_current_component->on_tertiary_button_release(x, y);
          break;

        default:
#ifdef DEBUG
          std::cout << "GUIManager:process_button_events: Got unhandled BUTTON_EVENT release: "
                    << button.id << std::endl;
#endif
          break;
        }
    }
}

void
GUIManager::process_events ()
{
  float const x = g_input_context->get_mouse_x();
  float const y = g_input_context->get_mouse_y();

  if (m_grabbing_component && (m_last_x != x || m_last_y != y))
    {
      m_grabbing_component->on_mouse_move (x, y, x - m_last_x, y - m_last_y);
    }
  if (m_current_component != m_grabbing_component)
    {
      m_current_component->on_mouse_move(x, y, x - m_last_x, y - m_last_y);
    }

  if (!m_grabbing_component)
    {
      m_current_component = find_component_at (x, y);

      if (m_last_component != m_current_component)
        {
          if (m_current_component) m_current_component->on_mouse_enter ();
          if (m_last_component)
            m_last_component->on_mouse_leave ();

          m_last_component = m_current_component;
        }
    }
  else
    {
      GUIComponent* comp = find_component_at (x, y);

      if (comp != m_grabbing_component)
        {
          m_grabbing_component->on_mouse_leave();
          m_last_component = comp;
        }
      else if (m_last_component != m_grabbing_component)
        {
          m_grabbing_component->on_mouse_enter();
        }
    }

  Event event;
  while (g_input_context->get_event (&event))
    {
      if (m_current_component)
        {
          switch (event.type)
            {
            case BUTTON_EVENT:
              //std::cout << "BUTTON_EVENT: " << event.button.id  << " state: " << event.button.pressed << std::endl;
              process_button_events (event.button);
              break;
            default:
              std::cout << "GUIManager: Unhandled event type" << std::endl;
              break;
            }
        }
    }

  m_last_x = x;
  m_last_y = y;
}

void
GUIManager::grab_mouse (GUIComponent* comp)
{
  m_grabbing_component = comp;
  m_current_component  = comp;
}

void
GUIManager::ungrab_mouse (GUIComponent* comp)
{
  m_grabbing_component = nullptr;
}

void
GUIManager::add(std::unique_ptr<GUIComponent> component)
{
  assert(component != nullptr);
  m_components.emplace_back(std::move(component));
}

/* EOF */
