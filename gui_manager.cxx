//  $Id$
//
//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include <config.h>
#include "string_utils.hxx"
#include "construo.hxx"
#include "graphic_context.hxx"
#include "input_context.hxx"
#include "system_context.hxx"
#include "controller.hxx"
#include "keep_alive_mgr.hxx"
#include "events.hxx"
#include "gui_component.hxx"
#include "gui_manager.hxx"
#include "worldview_component.hxx"
#include "worldview_insert_tool.hxx"

using namespace StringUtils;

GUIManager::GUIManager ()
{
  do_quit = false;

  frame_count = 0;
  start_time  = system_context->get_time ();

  last_component     = 0;
  grabbing_component = 0;
  current_component  = 0;

}

GUIManager::~GUIManager ()
{
}
  
void
GUIManager::run_once ()
{ 
  frame_count += 1;

  if (start_time + 3000 < system_context->get_time ())
    {
      float passed_time = (system_context->get_time () - start_time) / 1000.0f;
      
      std::cout << "FPS: " << frame_count / passed_time << std::endl;
      
      frame_count = 0;
      start_time  = system_context->get_time ();
    }

  process_events ();

  update();
  
  graphic_context->clear ();
  draw ();
  draw_overlay ();
  graphic_context->flip ();
}

void
GUIManager::draw ()
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      (*i)->draw (graphic_context);
    }
}

void
GUIManager::quit()
{
  do_quit = true;
}

GUIComponent*
GUIManager::find_component_at (int x, int y)
{
  GUIComponent* component = 0;
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x, y))
        component = *i;
    }
  return component;
}

void
GUIManager::process_button_events (ButtonEvent& button)
{
  int x = input_context->get_mouse_x();
  int y = input_context->get_mouse_y();

  if (button.pressed)
    {
      switch (button.id)
        {
        case BUTTON_START:
          Controller::instance()->start_simulation ();
          break;

        case BUTTON_PRIMARY:
          current_component->on_primary_button_press(x, y);
          break;

        case BUTTON_SECONDARY:
          current_component->on_secondary_button_press(x, y);
          break;

        case BUTTON_TERTIARY:
          current_component->on_tertiary_button_press(x, y);
          break;

        case BUTTON_FIX:
          current_component->on_fix_press (x, y);
          break;
        case BUTTON_DELETE:
          current_component->on_delete_press (x, y);
          break;
        case BUTTON_DUPLICATE:
          current_component->on_duplicate_press (x, y);
          break;

        case BUTTON_SCROLL_LEFT:
          current_component->scroll_left ();
          break;

        case BUTTON_SCROLL_RIGHT:
          current_component->scroll_right ();
          break;

        case BUTTON_SCROLL_UP:
          current_component->scroll_up ();
          break;

        case BUTTON_SCROLL_DOWN:
          current_component->scroll_down ();
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

        case BUTTON_ESCAPE:
          do_quit = true;
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
          current_component->wheel_down (x, y);
          break;

        case BUTTON_ZOOM_IN:
          current_component->wheel_up (x, y);
          break;

        default:
          current_component->on_button_press (button.id, x, y);
          break;
        }
    }
  else // button released
    {
      switch (button.id)
        {
        case BUTTON_PRIMARY:
          current_component->on_primary_button_release(x, y);
          break;

        case BUTTON_SECONDARY:
          current_component->on_secondary_button_release(x, y);
          break;

        case BUTTON_TERTIARY:
          current_component->on_tertiary_button_release(x, y);
          break;

        default:
          std::cout << "GUIManager:process_button_events: Got unhandled BUTTON_EVENT release: "
                    << button.id << std::endl;
          break;
        }
    }
}

void
GUIManager::process_events ()
{
  int x = input_context->get_mouse_x();
  int y = input_context->get_mouse_y();
      
  if (grabbing_component && (last_x != x || last_y != y))
    {
      grabbing_component->on_mouse_move (x, y, x - last_x, y - last_y);
    }
  if (current_component != grabbing_component)
    {
      current_component->on_mouse_move (x, y, x - last_x, y - last_y);
    }

  if (!grabbing_component)
    {
      current_component = find_component_at (x, y);
      
      if (last_component != current_component)
        {
          if (current_component) current_component->on_mouse_enter ();
          if (last_component)
            last_component->on_mouse_leave ();

          last_component = current_component;
        }
    }
  else
    {
      GUIComponent* comp = find_component_at (x, y);

      if (comp != grabbing_component)
        {
          grabbing_component->on_mouse_leave();
          last_component = comp;
        }
      else if (last_component != grabbing_component)
        {
          grabbing_component->on_mouse_enter();
        }
    }

  Event event;
  while (input_context->get_event (&event))
    {
      if (current_component)
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

  last_x = x;
  last_y = y;
}

void
GUIManager::grab_mouse (GUIComponent* comp)
{
  grabbing_component = comp;
  current_component  = comp;
}

void
GUIManager::ungrab_mouse (GUIComponent* comp)
{
  grabbing_component = 0;
}

void
GUIManager::add (GUIComponent* c)
{
  assert(c);
  components.push_back(c);
}

/* EOF */
