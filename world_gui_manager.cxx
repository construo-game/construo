//  $Id$
//
//  Construo - A wire-frame construction gamee
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
#include "controller.hxx"
#include "construo.hxx"
#include "graphic_context.hxx"
#include "worldview_component.hxx"
#include "string_utils.hxx"
#include "worldview_insert_tool.hxx"
#include "gui_buttons.hxx"
#include "gui_label.hxx"
#include "world_gui_manager.hxx"

using namespace StringUtils;

WorldGUIManager* WorldGUIManager::instance_ = 0;


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

void switch_to_collider_mode() {
  WorldViewComponent::instance()->set_mode (WorldViewComponent::COLLIDER_MODE);
}

void switch_to_select_mode() {
  WorldViewComponent::instance()->set_mode (WorldViewComponent::SELECT_MODE);
}

WorldGUIManager::WorldGUIManager ()
{
  instance_  = this;

  add(new WorldViewComponent ());
  add(new GUIRunButton ());
  add(new GUISlowMoButton  ());
  add(new GUIUndoButton ());
  add(new GUIZoomInButton ());
  add(new GUIZoomOutButton ());
  add(new GUILoadButton ());
  add(new GUIQuitButton ());

  add(new GUIGenericButton ("InsertMode", 700, 130, 90, 25, switch_to_insert_mode));
  add(new GUIGenericButton ("SelectMode", 700, 160, 90, 25, switch_to_select_mode));
  add(new GUIGenericButton ("ZoomMode",   700, 190, 90, 25, switch_to_zoom_mode));
  add(new GUIGenericButton ("ColliderMode",   700, 100, 90, 25, switch_to_collider_mode));

  add(new GUIGenericButton ("Increase ParticleMass",   650, 220, 140, 25, increase_particle_mass));
  add(new GUIGenericButton ("Decrease ParticleMass",   650, 250, 140, 25, decrease_particle_mass));

  add(new GUILabel ("Stiffness",   550, 280, 75, 25));

  add(new GUIGenericButton ("+",   650, 280, 25, 25, increase_particle_mass));
  add(new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));

  add(new GUIGenericButton ("+",   650, 280, 25, 25, increase_particle_mass));
  add(new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));
 
  /*
    GUIWindow* window = new GUIWindow ("Window Title", 300, 100, 300, 400);
    window->add (new GUIButton ("Testbutton", 10, 10, 100, 25));
    add (window);
  */
}

WorldGUIManager::~WorldGUIManager ()
{
}

void
WorldGUIManager::draw_overlay ()
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

/* EOF */
