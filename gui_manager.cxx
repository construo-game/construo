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
#include "construo.hxx"
#include "graphic_context.hxx"
#include "input_context.hxx"
#include "system_context.hxx"
#include "controller.hxx"
#include "keep_alive_mgr.hxx"
#include "events.hxx"
#include "gui_component.hxx"
#include "gui_manager.hxx"
#include "gui_window.hxx"
#include "worldview_component.hxx"
#include "gui_buttons.hxx"

extern Controller* controller;

WorldViewComponent* worldview_component;

GUIManager::GUIManager ()
{
  do_quit = false;

  last_component     = 0;
  grabbing_component = 0;
  current_component  = 0;

  worldview_component = new WorldViewComponent ();
  components.push_back (worldview_component);

  components.push_back (new GUIRunButton ());
  components.push_back (new GUISlowMoButton  ());
  components.push_back (new GUIUndoButton ());
  components.push_back (new GUIZoomInButton ());
  components.push_back (new GUIZoomOutButton ());
  components.push_back (new GUIQuitButton ());

  /*
    GUIWindow* window = new GUIWindow ("Window Title", 300, 100, 300, 400);
    window->add (new GUIButton ("Testbutton", 10, 10, 100, 25));
    components.push_back (window);
  */
}
  
void
GUIManager::run ()
{ 
  while (!do_quit)
    {
      process_events ();

      controller->update ();
      graphic_context->clear ();
      
      draw_status();

      for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
        {
          (*i)->draw (graphic_context);
        }

      graphic_context->flip ();
      //KeepAliveMgr::keep_alive ();
      if (controller->is_running())
        {
          system_context->sleep (10000); // limit CPU usage via brute force
          input_context->wait_for_events();
        }
      else
        {
          input_context->wait_for_events_blocking();
        }
    }
}

void
GUIManager::draw_status ()
{
  graphic_context->draw_string (10, 20, "..:: Construo V"VERSION" ::..");
  graphic_context->draw_string (10, 32, "=========================");
   
  graphic_context->draw_string (400, 20, "      [1-9] - quick save");
  graphic_context->draw_string (400, 32, "[shift 1-9] - quick load");
  graphic_context->draw_string (400, 44, "   [escape] - quit");
  graphic_context->draw_string (400, 56, "    [space] - toggle slow motion");

  graphic_context->draw_string (600,  20, "  [left] - insert/connect spots");
  graphic_context->draw_string (600,  32, "[middle] - start/stop simulation");
  graphic_context->draw_string (600,  44, " [right] - remove spot");
  graphic_context->draw_string (600,  56, "     [c] - clear screen");
  graphic_context->draw_string (600,  68, "     [f] - fix current spot");
  graphic_context->draw_string (600,  80, "     [u] - undo to last state");
  graphic_context->draw_string (600,  92, "     [r] - redo (undo an undo)");

  if (controller->is_running ())
    graphic_context->draw_string (graphic_context->get_width () - 60,
                                  graphic_context->get_height () - 10,
                                  "[RUNNING]", Color(0xFF0000));
  else
    graphic_context->draw_string (graphic_context->get_width () - 60,
                                  graphic_context->get_height () - 10,
                                  "[STOPPED]", Color(0x00FF00));

  if (controller->slow_down_active ())
    {
      graphic_context->draw_string (10,
                                    graphic_context->get_height () - 10,
                                    "[SLOW-MOTION]", Color(0x00FFFF));
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
          controller->start_simulation ();
          break;

        case BUTTON_PRIMARY:
          current_component->on_primary_button_press(x, y);
          break;

        case BUTTON_SECONDARY:
          current_component->on_secondary_button_press(x, y);
          break;

        case BUTTON_FIX:
          current_component->on_fix_press (x, y);
          break;
        case BUTTON_DELETE:
          current_component->on_delete_press (x, y);
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
          controller->clear_world ();
          break;
                  
        case BUTTON_UNDO:
          controller->undo ();
          break;
                  
        case BUTTON_REDO:
          controller->redo ();
          break;

        case BUTTON_ESCAPE:
          do_quit = true;
          break;

        case BUTTON_TOGGLESLOWMO:
          controller->set_slow_down (!controller->slow_down_active ());
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
          controller->save_to_slot (button.id - BUTTON_QUICKSAVE0);
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
          controller->load_from_slot (button.id - BUTTON_QUICKLOAD0);
          break;

        default:
          std::cout << "Got unhandled BUTTON_EVENT press: " << button.id << std::endl;
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

        case BUTTON_ZOOM_OUT:
          current_component->wheel_up (x, y);
          break;

        case BUTTON_ZOOM_IN:
          current_component->wheel_down (x, y);
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
  Event event;

  int x = input_context->get_mouse_x();
  int y = input_context->get_mouse_y();
      
  if (grabbing_component && (last_x != x || last_y != y))
    {
      grabbing_component->on_mouse_move (x, y, x - last_x, y - last_y);
    }
      
  last_x = x;
  last_y = y;

  if (!grabbing_component)
    {
      current_component = find_component_at (x, y);
      assert (current_component);

      if (last_component != current_component)
        {
          current_component->on_mouse_enter ();
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

  while (input_context->get_event (&event))
    {
      if (current_component)
        {
          switch (event.type)
            {
            case BUTTON_EVENT:
              process_button_events (event.button);
              break;
            default: 
              std::cout << "GUIManager: Unhandled event type" << std::endl;
              break;
            }
        }
    }
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

/* EOF */
