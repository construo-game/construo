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

#include "gui_manager.hpp"

#include <logmich/log.hpp>

#include "construo.hpp"
#include "graphic_context.hpp"
#include "input_context.hpp"
#include "system_context.hpp"
#include "controller.hpp"
#include "events.hpp"
#include "gui_widget.hpp"
#include "worldview_widget.hpp"
#include "worldview_insert_tool.hpp"
#include "root_graphic_context.hpp"
#include "screen_manager.hpp"

GUIManager::GUIManager() :
  m_frame_count(0),
  m_start_time(g_system_context->get_time()),
  m_current_fps(0.0f),
  m_last_widget(nullptr),
  m_current_widget(nullptr),
  m_grabbing_widget(nullptr),
  m_previous_pos(),
  m_widgets()
{
}

GUIManager::~GUIManager ()
{
}

void
GUIManager::run_once(GraphicContext& gc)
{
  m_frame_count += 1;

  if (m_start_time + 100 < g_system_context->get_time ())
  {
    float const passed_time = static_cast<float>(g_system_context->get_time() - m_start_time) / 1000.0f;

    m_current_fps = static_cast<float>(m_frame_count) / passed_time;

    m_frame_count = 0;
    m_start_time  = g_system_context->get_time ();
  }

  process_events ();

  update();

  draw(gc);
}

void
GUIManager::draw(GraphicContext& gc)
{
  gc.clear();
  for (auto i = m_widgets.begin (); i != m_widgets.end (); ++i)
  {
    (*i)->draw(gc);
  }

  draw_overlay(gc);
  gc.flip();
}

GUIWidget*
GUIManager::find_widget_at(geom::fpoint const& pos) const
{
  GUIWidget* widget = nullptr;
  for (auto i = m_widgets.begin (); i != m_widgets.end (); ++i)
  {
    if ((*i)->is_at(pos)) {
      widget = i->get();
    }
  }
  return widget;
}

void
GUIManager::process_button_events (ButtonEvent& button)
{
  geom::fpoint const pos = g_input_context->get_mouse_pos();

  if (button.pressed)
  {
    switch (button.id)
    {
      case Action::FULLSCREEN:
        g_graphic_context->toggle_fullscreen();
        break;

      case Action::START:
        Controller::instance()->start_simulation();
        break;

      case Action::PRIMARY:
        m_current_widget->on_primary_button_press(pos);
        break;

      case Action::SECONDARY:
        m_current_widget->on_secondary_button_press(pos);
        break;

      case Action::TERTIARY:
        m_current_widget->on_tertiary_button_press(pos);
        break;

      case Action::SCALE:
        m_current_widget->on_scale_press(pos);
        break;

      case Action::FIX:
        m_current_widget->on_fix_press(pos);
        break;

      case Action::JOIN:
        m_current_widget->on_join_press(pos);
        break;

      case Action::GRID:
        m_current_widget->on_grid_press(pos);
        break;

      case Action::DELETE:
        m_current_widget->on_delete_press(pos);
        break;
      case Action::DUPLICATE:
        m_current_widget->on_duplicate_press(pos);
        break;

      case Action::SCROLL_LEFT:
        m_current_widget->scroll_left();
        break;

      case Action::SCROLL_RIGHT:
        m_current_widget->scroll_right();
        break;

      case Action::SCROLL_UP:
        m_current_widget->scroll_up();
        break;

      case Action::SCROLL_DOWN:
        m_current_widget->scroll_down();
        break;

      case Action::CLEAR:
        Controller::instance()->clear_world();
        break;

      case Action::UNDO:
        Controller::instance()->undo();
        break;

      case Action::REDO:
        Controller::instance()->redo();
        break;

      case Action::ACTIONCAM:
        Controller::instance()->set_action_cam(!Controller::instance()->get_action_cam());
        break;

      case Action::HIDEDOTS:
        Controller::instance()->set_hide_dots (!Controller::instance()->get_hide_dots());
        break;

      case Action::ESCAPE:
        ScreenManager::instance()->quit();
        break;

      case Action::MODE_CHANGE:
        if (WorldViewWidget::instance()->get_mode () == WorldViewWidget::INSERT_MODE)
        {
          WorldViewWidget::instance()->set_mode(WorldViewWidget::SELECT_MODE);
        }
        else
        {
          WorldViewWidget::instance()->set_mode(WorldViewWidget::INSERT_MODE);
        }
        break;

      case Action::TOGGLESLOWMO:
        Controller::instance()->set_slow_down (!Controller::instance()->slow_down_active());
        break;

      case Action::RUN:
        Controller::instance()->start_simulation();
        break;

      case Action::QUICKSAVE0: Controller::instance()->save_to_slot(0); break;
      case Action::QUICKSAVE1: Controller::instance()->save_to_slot(1); break;
      case Action::QUICKSAVE2: Controller::instance()->save_to_slot(2); break;
      case Action::QUICKSAVE3: Controller::instance()->save_to_slot(3); break;
      case Action::QUICKSAVE4: Controller::instance()->save_to_slot(4); break;
      case Action::QUICKSAVE5: Controller::instance()->save_to_slot(5); break;
      case Action::QUICKSAVE6: Controller::instance()->save_to_slot(6); break;
      case Action::QUICKSAVE7: Controller::instance()->save_to_slot(7); break;
      case Action::QUICKSAVE8: Controller::instance()->save_to_slot(8); break;
      case Action::QUICKSAVE9: Controller::instance()->save_to_slot(9); break;

      case Action::QUICKLOAD0: Controller::instance()->load_from_slot(0); break;
      case Action::QUICKLOAD1: Controller::instance()->load_from_slot(1); break;
      case Action::QUICKLOAD2: Controller::instance()->load_from_slot(2); break;
      case Action::QUICKLOAD3: Controller::instance()->load_from_slot(3); break;
      case Action::QUICKLOAD4: Controller::instance()->load_from_slot(4); break;
      case Action::QUICKLOAD5: Controller::instance()->load_from_slot(5); break;
      case Action::QUICKLOAD6: Controller::instance()->load_from_slot(6); break;
      case Action::QUICKLOAD7: Controller::instance()->load_from_slot(7); break;
      case Action::QUICKLOAD8: Controller::instance()->load_from_slot(8); break;
      case Action::QUICKLOAD9: Controller::instance()->load_from_slot(9); break;

      case Action::ZOOM_OUT:
        m_current_widget->wheel_down(pos);
        break;

      case Action::ZOOM_IN:
        m_current_widget->wheel_up(pos);
        break;

      default:
        m_current_widget->on_button_press(static_cast<int>(button.id), pos);
        break;
    }
  }
  else // button released
  {
    switch (button.id)
    {
      case Action::PRIMARY:
        m_current_widget->on_primary_button_release(pos);
        break;

      case Action::SECONDARY:
        m_current_widget->on_secondary_button_release(pos);
        break;

      case Action::TERTIARY:
        m_current_widget->on_tertiary_button_release(pos);
        break;

      default:
        log_debug("GUIManager:process_button_events: Got unhandled Action::EVENT release: {}", button.id);
        break;
    }
  }
}

void
GUIManager::process_events()
{
  geom::fpoint const pos = g_input_context->get_mouse_pos();

  if (m_grabbing_widget && m_previous_pos != pos)
  {
    m_grabbing_widget->on_mouse_move(pos, geom::foffset(pos.as_vec() - m_previous_pos.as_vec()));
  }
  if (m_current_widget != m_grabbing_widget)
  {
    m_current_widget->on_mouse_move(pos, geom::foffset(pos.x() - m_previous_pos.x(),
                                                          pos.y() - m_previous_pos.y()));
  }

  if (!m_grabbing_widget)
  {
    m_current_widget = find_widget_at(pos);

    if (m_last_widget != m_current_widget)
    {
      if (m_current_widget) m_current_widget->on_mouse_enter();
      if (m_last_widget)
        m_last_widget->on_mouse_leave();

      m_last_widget = m_current_widget;
    }
  }
  else
  {
    GUIWidget* comp = find_widget_at(pos);

    if (comp != m_grabbing_widget)
    {
      m_grabbing_widget->on_mouse_leave();
      m_last_widget = comp;
    }
    else if (m_last_widget != m_grabbing_widget)
    {
      m_grabbing_widget->on_mouse_enter();
    }
  }

  Event event;
  while (g_input_context->get_event(&event))
  {
    if (m_current_widget)
    {
      switch (event.type)
      {
        case BUTTON_EVENT:
          process_button_events (event.button);
          break;

        default:
          log_debug("GUIManager: Unhandled event type: {}", event.type);
          break;
      }
    }
  }

  m_previous_pos = pos;
}

void
GUIManager::grab_mouse(GUIWidget& widget)
{
  m_grabbing_widget = &widget;
  m_current_widget  = &widget;
}

void
GUIManager::ungrab_mouse(GUIWidget& widget)
{
  if (m_grabbing_widget != &widget) {
    log_error("GUIManager: ungrab mismatch: {} != {}", static_cast<void*>(m_grabbing_widget), static_cast<void*>(&widget));
  }

  m_grabbing_widget = nullptr;
}

void
GUIManager::add(std::unique_ptr<GUIWidget> widget)
{
  assert(widget != nullptr);
  m_widgets.emplace_back(std::move(widget));
}

/* EOF */
