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
#include "gui_window.hxx"
#include "gui_label.hxx"
#include "globals.hxx"
#include "screen_manager.hxx"
#include "root_graphic_context.hxx"
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

bool insert_mode_hfunc() {
  return WorldViewComponent::instance()->get_mode() == WorldViewComponent::INSERT_MODE;
}

bool zoom_mode_hfunc() {
  return WorldViewComponent::instance()->get_mode() == WorldViewComponent::ZOOM_MODE;
}

bool select_mode_hfunc() {
  return WorldViewComponent::instance()->get_mode() == WorldViewComponent::SELECT_MODE;
}

bool collider_mode_hfunc() {
  return WorldViewComponent::instance()->get_mode() == WorldViewComponent::COLLIDER_MODE;
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

void save_button_callback() {
  ScreenManager::instance()->set_gui(ScreenManager::SAVE_GUI);
}

void action_cam_callback() {
  Controller::instance()->set_action_cam(!Controller::instance()->get_action_cam());
}

void zoom_in_callback() {
  WorldViewComponent::instance()->wheel_up (graphic_context->get_width()/2,
                                            graphic_context->get_height()/2);
}

void zoom_out_callback() {
  WorldViewComponent::instance()->wheel_down (graphic_context->get_width()/2,
                                              graphic_context->get_height()/2);

}

void hide_dots_callback() {
  Controller& c = *Controller::instance();
  c.set_hide_dots(!c.get_hide_dots());
}

bool hide_dots_hfunc ()
{
  return Controller::instance()->get_hide_dots();
}

bool action_cam_hfunc ()
{
  return Controller::instance()->get_action_cam();
}

bool show_grid_hfunc() 
{
  return WorldViewComponent::instance()->uses_grid();
}

void show_grid_callback()
{
  WorldViewComponent::instance()->on_grid_press(0, 0);
}

void redo_callback ()
{
  return Controller::instance()->redo();
}

void undo_callback ()
{
  return Controller::instance()->undo();
}

#define BUTTON_POS(n) (80 + n * 30)
#define BUTTON_RPOS(n) (50 + n * 30)
#define BUTTON_WIDTH  75
#define BUTTON_HEIGHT 25
#define BUTTON_LX_POS (graphic_context->get_width() - BUTTON_WIDTH - 10)

WorldGUIManager::WorldGUIManager ()
{
  instance_  = this;

  add(new WorldViewComponent ());
  //add(new GUILabel ("Sim:", 10, 50, 10, 10));
  add(new GUIRunButton ());
  add(new GUISlowMoButton  ());
  //add(new GUIZoomInButton ());
  //add(new GUIZoomOutButton ());
  add(new GUILoadButton ());
  add(new GUIGenericButton ("Save", 10, BUTTON_POS(10), BUTTON_WIDTH, BUTTON_HEIGHT, save_button_callback));

  add(new GUIGenericButton ("Undo", 10, BUTTON_POS(6), BUTTON_WIDTH, BUTTON_HEIGHT, undo_callback));
  add(new GUIGenericButton ("Redo", 10, BUTTON_POS(7), BUTTON_WIDTH, BUTTON_HEIGHT, redo_callback));

  add(new GUIGenericButton ("ActionCam", 10, BUTTON_POS(2), BUTTON_WIDTH, BUTTON_HEIGHT, action_cam_callback, action_cam_hfunc));
  add(new GUIGenericButton ("Hide Dots", 10, BUTTON_POS(3), BUTTON_WIDTH, BUTTON_HEIGHT, hide_dots_callback, hide_dots_hfunc));
  add(new GUIGenericButton ("Use Grid", 10, BUTTON_POS(4), BUTTON_WIDTH, BUTTON_HEIGHT, show_grid_callback, show_grid_hfunc));
  add(new GUIQuitButton ());

  //add(new GUILabel ("Tools", BUTTON_LX_POS, BUTTON_POS(3)+5, BUTTON_WIDTH, BUTTON_HEIGHT));

  add(new GUIGenericButton ("Insert", BUTTON_LX_POS, BUTTON_RPOS(4), BUTTON_WIDTH, BUTTON_HEIGHT, switch_to_insert_mode, insert_mode_hfunc));
  add(new GUIGenericButton ("Select", BUTTON_LX_POS, BUTTON_RPOS(5), BUTTON_WIDTH, BUTTON_HEIGHT, switch_to_select_mode, select_mode_hfunc));
  add(new GUIGenericButton ("Collider",   BUTTON_LX_POS, BUTTON_RPOS(6), BUTTON_WIDTH, BUTTON_HEIGHT, switch_to_collider_mode, collider_mode_hfunc));
  add(new GUIGenericButton ("Zoom",   BUTTON_LX_POS, BUTTON_RPOS(7), BUTTON_WIDTH, BUTTON_HEIGHT, switch_to_zoom_mode, zoom_mode_hfunc));

  add(new GUIGenericButton ("-", BUTTON_LX_POS + 38, BUTTON_RPOS(8), 25, 25, zoom_out_callback));
  add(new GUIGenericButton ("+", BUTTON_LX_POS +  6, BUTTON_RPOS(8), 25, 25, zoom_in_callback));

  // FIXME: Stuff for particle mass and Co. must be implemented in another way
  if(0)
    {
      add(new GUIGenericButton ("Increase ParticleMass",   650, 220, 140, 25, increase_particle_mass));
      add(new GUIGenericButton ("Decrease ParticleMass",   650, 250, 140, 25, decrease_particle_mass));

      add(new GUILabel ("Stiffness",   550, 280, 75, 25));

      add(new GUIGenericButton ("+",   BUTTON_LX_POS, 280, 25, 25, increase_particle_mass));
      add(new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));

      add(new GUIGenericButton ("+",   650, 280, 25, 25, increase_particle_mass));
      add(new GUIGenericButton ("-",   680, 280, 25, 25, decrease_particle_mass));
    }
  //add(new GUIWindow ("Test Window",   200, 100, 200, 90));
 
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
WorldGUIManager::update()
{
  Controller::instance()->update ();
}

void
WorldGUIManager::draw_overlay ()
{
  graphic_context->draw_string (10,  20, "      [1-9] - quick save");
  graphic_context->draw_string (10,  32, "[shift 1-9] - quick load");
  graphic_context->draw_string (10,  44, "    [space] - run simulation");
  graphic_context->draw_string (10,  56, "      [tab] - toggle slow motion");
  graphic_context->draw_string (10,  68, "      [F11] - toggle fullscreen");

  graphic_context->draw_string (200,  20, "     [c] - clear scene");
  graphic_context->draw_string (200,  32, "     [u] - undo to last state");
  graphic_context->draw_string (200,  44, "     [r] - redo (undo an undo)");
  graphic_context->draw_string (200,  56, "   [+/-] - zoom in/out");
  graphic_context->draw_string (200,  68, "     [g] - toggle grid");

  graphic_context->draw_string (600,  32, "[middle] - scroll");

  switch (WorldViewComponent::instance()->get_mode())
    {
    case WorldViewComponent::INSERT_MODE:
      graphic_context->draw_string (600,  20, "  [left] - insert/connect spots");
      graphic_context->draw_string (600,  44, " [right] - remove spot");
      graphic_context->draw_string (400,  20, "     [f] - fix current dot");
      break;

    case WorldViewComponent::SELECT_MODE:
      graphic_context->draw_string (600,  20, "  [left] - create/move selection");
      graphic_context->draw_string (600,  44, " [right] - rotate selection");

      graphic_context->draw_string (400,  20, "     [v] - set velocity");
      graphic_context->draw_string (400,  32, "     [d] - duplicate selection");
      graphic_context->draw_string (400,  44, "     [h] - flip selection");
      graphic_context->draw_string (400,  56, "     [f] - fix selection");
      graphic_context->draw_string (400,  68, "     [j] - join dots");
      break;
      
    case WorldViewComponent::ZOOM_MODE:
      graphic_context->draw_string (600,  20, "  [left] - zoom into region");
      graphic_context->draw_string (600,  44, " [right] - zoom out");
      break;

    case WorldViewComponent::COLLIDER_MODE:
      graphic_context->draw_string (600,  20, "  [left] - create/move collider");
      graphic_context->draw_string (600,  44, " [right] - remove collider");
      break;

    default:
      break;
    }

  World& world = *Controller::instance()->get_world ();

  /*graphic_context->draw_string (600,  430, "Particles Mass: ");
    graphic_context->draw_string (BUTTON_LX_POS,  430, 
    to_string(WorldViewComponent::instance()->get_insert_tool()->get_particle_mass ()));
  */
  int bottom_line = graphic_context->get_height() - 10;
  graphic_context->draw_string (10, bottom_line-20, "FPS: ");
  graphic_context->draw_string (80, bottom_line-20, to_string(current_fps));

  graphic_context->draw_string (10, bottom_line, "Pos: ");
  graphic_context->draw_string (80, bottom_line, 
                                to_string(WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos())));

  graphic_context->draw_string (210, bottom_line-20, "Particles: ");
  graphic_context->draw_string (280, bottom_line-20, to_string(world.get_num_particles()));

  graphic_context->draw_string (210, bottom_line, "Springs: ");
  graphic_context->draw_string (280, bottom_line, to_string(world.get_num_springs()));

  graphic_context->draw_string (410, bottom_line, "Zoom: ");
  graphic_context->draw_string (480, bottom_line, to_string(WorldViewComponent::instance()->get_zoom()));

  graphic_context->draw_string (610, bottom_line, "..:: Construo V"VERSION" ::..");
  //graphic_context->draw_string (680, bottom_line, to_string(WorldViewComponent::instance()->get_zoom()));
}

/* EOF */
