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
#include "gui_window.hxx"
#include "worldview_component.hxx"
#include "worldview_insert_tool.hxx"
#include "gui_buttons.hxx"
#include "gui_label.hxx"

using namespace StringUtils;

GUIManager* GUIManager::instance_;

void increase_particle_mass() {
  WorldViewInsertTool& wc = *WorldViewComponent::instance()->get_insert_tool();
  wc.set_particle_mass(wc.get_particle_mass() + 1.0f);
}

void decrease_particle_mass() {
  WorldViewInsertTool& wc = *WorldViewComponent::instance()->get_insert_tool();
  wc.set_particle_mass(wc.get_particle_mass() - 1.0f);
}

void switch_to_insert_mode() {
  WorldViewComponent::instance()->set_mode (WorldViewComponent::INSERT_MODE);
}

void switch_to_zoom_mode() {
  WorldViewComponent::instance()->set_mode (WorldViewComponent::ZOOM_MODE);
}

void switch_to_select_mode() {
  WorldViewComponent::instance()->set_mode (WorldViewComponent::SELECT_MODE);
}


GUIManager::GUIManager ()
{
  instance_ = this;
  do_quit = false;

  frame_count = 0;
  start_time  = system_context->get_time ();

  last_component     = 0;
  grabbing_component = 0;
  current_component  = 0;

  components.push_back (new WorldViewComponent ());
  components.push_back (new GUIRunButton ());
  components.push_back (new GUISlowMoButton  ());
  components.push_back (new GUIUndoButton ());
  components.push_back (new GUIZoomInButton ());
  components.push_back (new GUIZoomOutButton ());
  components.push_back (new GUIQuitButton ());

  components.push_back (new GUIGenericButton ("InsertMode", 700, 130, 90, 25, switch_to_insert_mode));
  components.push_back (new GUIGenericButton ("SelectMode", 700, 160, 90, 25, switch_to_select_mode));
  components.push_back (new GUIGenericButton ("ZoomMode",   700, 190, 90, 25, switch_to_zoom_mode));

  components.push_back (new GUIGenericButton ("Increase ParticleMass",   650, 220, 140, 25, increase_particle_mass));
  components.push_back (new GUIGenericButton ("Decrease ParticleMass",   650, 250, 140, 25, decrease_particle_mass));

  components.push_back (new GUILabel ("Stiffness",   550, 280, 75, 25));

  components.push_back (new GUIGenericButton ("+",   650, 280, 25, 25, increase_particle_mass));
  components.push_back (new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));

  components.push_back (new GUIGenericButton ("+",   650, 280, 25, 25, increase_particle_mass));
  components.push_back (new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));
 
  /*
    GUIWindow* window = new GUIWindow ("Window Title", 300, 100, 300, 400);
    window->add (new GUIButton ("Testbutton", 10, 10, 100, 25));
    components.push_back (window);
  */
}

GUIManager::~GUIManager ()
{
  instance_ = 0;
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

  Controller::instance()->update ();
  graphic_context->clear ();
      
  draw_status();

  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      (*i)->draw (graphic_context);
    }

  file_manager.draw ();
          
  graphic_context->flip ();
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

  World& world = *Controller::instance()->get_world ();

  graphic_context->draw_string (600,  430, "Particles Mass: ");
  graphic_context->draw_string (700,  430, 
                                to_string(WorldViewComponent::instance()->get_insert_tool()->get_particle_mass ()));

  graphic_context->draw_string (700,  530, "Particles: ");
  graphic_context->draw_string (770,  530, to_string(world.get_num_particles()));

  graphic_context->draw_string (700,  545, "Springs: ");
  graphic_context->draw_string (770,  545, to_string(world.get_num_springs()));

  graphic_context->draw_string (700,  560, "Zoom: ");
  graphic_context->draw_string (770,  560, to_string(WorldViewComponent::instance()->get_zoom()));

  if (Controller::instance()->slow_down_active ())
    {
      graphic_context->draw_string (10,
                                    graphic_context->get_height () - 10,
                                    "[SLOW-MOTION]", Color(0x00FFFFFF));
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

  Event event;
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
