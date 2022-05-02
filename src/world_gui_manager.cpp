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

#include "controller.hpp"
#include "construo.hpp"
#include "graphic_context.hpp"
#include "worldview_component.hpp"
#include "string_utils.hpp"
#include "worldview_insert_tool.hpp"
#include "gui_buttons.hpp"
#include "gui_window.hpp"
#include "gui_label.hpp"
#include "screen_manager.hpp"
#include "root_graphic_context.hpp"
#include "world_gui_manager.hpp"

using namespace StringUtils;

WorldGUIManager* WorldGUIManager::instance_ = nullptr;


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
  WorldViewComponent::instance()->wheel_up (g_graphic_context->get_width()/2,
                                            g_graphic_context->get_height()/2);
}

void zoom_out_callback() {
  WorldViewComponent::instance()->wheel_down (g_graphic_context->get_width()/2,
                                              g_graphic_context->get_height()/2);

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

WorldGUIManager::WorldGUIManager() :
  m_worldview_component(),
  m_run_button(),
  m_slowmo_button(),
  m_load_button(),
  m_save_button(),
  m_undo_button(),
  m_redo_button(),
  m_actioncam_button(),
  m_dots_button(),
  m_grid_button(),
  m_quit_button(),
  m_insert_button(),
  m_select_button(),
  m_collider_button(),
  m_zoom_button(),
  m_zoomout_button(),
  m_zoomin_button()
{
  instance_  = this;

  m_worldview_component = create<WorldViewComponent>();

  // simulation
  m_run_button = create<GUIRunButton>();
  m_slowmo_button = create<GUISlowMoButton>();
  m_load_button = create<GUILoadButton>();
  m_save_button = create<GUIGenericButton>("Save", save_button_callback);

  m_undo_button = create<GUIGenericButton>("Undo", undo_callback);
  m_redo_button = create<GUIGenericButton>("Redo", redo_callback);

  m_actioncam_button = create<GUIGenericButton>("ActionCam", action_cam_callback, action_cam_hfunc);
  m_dots_button = create<GUIGenericButton>("Hide Dots", hide_dots_callback, hide_dots_hfunc);
  m_grid_button = create<GUIGenericButton>("Use Grid", show_grid_callback, show_grid_hfunc);
  m_quit_button = create<GUIQuitButton>();

  // toolbar
  m_insert_button = create<GUIGenericButton>("Insert", switch_to_insert_mode, insert_mode_hfunc);
  m_select_button = create<GUIGenericButton>("Select", switch_to_select_mode, select_mode_hfunc);
  m_collider_button = create<GUIGenericButton>("Collider", switch_to_collider_mode, collider_mode_hfunc);
  m_zoom_button = create<GUIGenericButton>("Zoom", switch_to_zoom_mode, zoom_mode_hfunc);

  m_zoomout_button = create<GUIGenericButton>("-", zoom_out_callback);
  m_zoomin_button = create<GUIGenericButton>("+", zoom_in_callback);

  // FIXME: Stuff for particle mass and Co. must be implemented in another way
#if 0
  {
    create<GUIGenericButton>("Increase ParticleMass",   650, 220, 140, 25, increase_particle_mass);
    create<GUIGenericButton>("Decrease ParticleMass",   650, 250, 140, 25, decrease_particle_mass);

    create<GUILabel>("Stiffness",   550, 280, 75, 25);

    create<GUIGenericButton>("+",   BUTTON_LX_POS, 280, 25, 25, increase_particle_mass);
    create<GUIGenericButton>("-",   680, 280, 25, 25, decrease_particle_mass);

    create<GUIGenericButton>("+",   650, 280, 25, 25, increase_particle_mass);
    create<GUIGenericButton>("-",   680, 280, 25, 25, decrease_particle_mass);
  }
#endif
  //create<GUIWindow>("Test Window",   200, 100, 200, 90);

  /*
    GUIWindow* window = new GUIWindow ("Window Title", 300, 100, 300, 400);
    window->add (new GUIButton ("Testbutton", 10, 10, 100, 25);
    add (window);
  */

  resize(g_graphic_context->get_width(),
         g_graphic_context->get_height());
}

WorldGUIManager::~WorldGUIManager ()
{
}

void
WorldGUIManager::resize(float width, float height)
{
  GUIManager::resize(width, height);

  auto BUTTON_POS = [](int n) { return 80.0f + static_cast<float>(n) * 30.0f; };
  auto BUTTON_RPOS = [](int n) { return 50.0f + static_cast<float>(n) * 30.0f; };
  float const BUTTON_WIDTH = 75.0f;
  float const BUTTON_HEIGHT = 25.0f;
  float const BUTTON_LX_POS = width - BUTTON_WIDTH - 10.0f;

  m_worldview_component->set_geometry(0, 0, width, height);

  m_run_button->set_geometry(10, BUTTON_POS(0), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_slowmo_button->set_geometry(10, BUTTON_POS(1), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_load_button->set_geometry(10, BUTTON_POS(9), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_save_button->set_geometry(10, BUTTON_POS(10), BUTTON_WIDTH, BUTTON_HEIGHT);

  m_undo_button->set_geometry(10, BUTTON_POS(6), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_redo_button->set_geometry(10, BUTTON_POS(7), BUTTON_WIDTH, BUTTON_HEIGHT);

  m_actioncam_button->set_geometry(10, BUTTON_POS(2), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_dots_button->set_geometry(10, BUTTON_POS(3), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_grid_button->set_geometry(10, BUTTON_POS(4), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_quit_button->set_geometry(10, BUTTON_POS(12), BUTTON_WIDTH, BUTTON_HEIGHT);

  m_insert_button->set_geometry(BUTTON_LX_POS, BUTTON_RPOS(4), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_select_button->set_geometry(BUTTON_LX_POS, BUTTON_RPOS(5), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_collider_button->set_geometry(BUTTON_LX_POS, BUTTON_RPOS(6), BUTTON_WIDTH, BUTTON_HEIGHT);
  m_zoom_button->set_geometry(BUTTON_LX_POS, BUTTON_RPOS(7), BUTTON_WIDTH, BUTTON_HEIGHT);

  m_zoomout_button->set_geometry(BUTTON_LX_POS + 38, BUTTON_RPOS(8), 25, 25);
  m_zoomin_button->set_geometry(BUTTON_LX_POS +  6, BUTTON_RPOS(8), 25, 25);
}

void
WorldGUIManager::update()
{
  Controller::instance()->update ();
}

void
WorldGUIManager::draw_overlay(GraphicContext& gc)
{
  gc.draw_string (10,  20, "      [1-9] - quick save");
  gc.draw_string (10,  32, "[shift 1-9] - quick load");
  gc.draw_string (10,  44, "    [space] - run simulation");
  gc.draw_string (10,  56, "      [tab] - toggle slow motion");
  gc.draw_string (10,  68, "      [F11] - toggle fullscreen");

  gc.draw_string (200,  20, "     [c] - clear scene");
  gc.draw_string (200,  32, "     [u] - undo to last state");
  gc.draw_string (200,  44, "     [r] - redo (undo an undo)");
  gc.draw_string (200,  56, "   [+/-] - zoom in/out");
  gc.draw_string (200,  68, "     [g] - toggle grid");

  gc.draw_string (600,  32, "[middle] - scroll");

  switch (WorldViewComponent::instance()->get_mode())
    {
    case WorldViewComponent::INSERT_MODE:
      gc.draw_string (600,  20, "  [left] - insert/connect spots");
      gc.draw_string (600,  44, " [right] - remove spot");
      gc.draw_string (400,  20, "     [f] - fix current dot");
      break;

    case WorldViewComponent::SELECT_MODE:
      gc.draw_string (600,  20, "  [left] - create/move selection");
      gc.draw_string (600,  44, " [right] - rotate selection");

      gc.draw_string (400,  20, "     [v] - set velocity");
      gc.draw_string (400,  32, "     [d] - duplicate selection");
      gc.draw_string (400,  44, "     [h] - flip selection");
      gc.draw_string (400,  56, "     [f] - fix selection");
      gc.draw_string (400,  68, "     [j] - join dots");
      gc.draw_string (400,  80, "     [s] - scale selection");
      break;

    case WorldViewComponent::ZOOM_MODE:
      gc.draw_string (600,  20, "  [left] - zoom into region");
      gc.draw_string (600,  44, " [right] - zoom out");
      break;

    case WorldViewComponent::COLLIDER_MODE:
      gc.draw_string (600,  20, "  [left] - create/move collider");
      gc.draw_string (600,  44, " [right] - remove collider");
      break;

    default:
      break;
    }

  World& world = *Controller::instance()->get_world ();

  /*graphic_context->draw_string (600,  430, "Particles Mass: ");
    graphic_context->draw_string (BUTTON_LX_POS,  430,
    to_string(WorldViewComponent::instance()->get_insert_tool()->get_particle_mass ()));
  */
  float const bottom_line = gc.get_height() - 10;
  gc.draw_string(10.0f, bottom_line - 20.0f, "FPS: ");
  gc.draw_string(80.0f, bottom_line - 20.0f, to_string(get_fps()));

  gc.draw_string(10.0f, bottom_line, "Pos: ");
  gc.draw_string(80.0f, bottom_line,
                                to_string(WorldViewComponent::instance()->zoom().screen_to_world(g_input_context->get_mouse_pos())));

  gc.draw_string(210.0f, bottom_line - 20.0f, "Particles: ");
  gc.draw_string(280.0f, bottom_line - 20.0f, to_string(world.get_num_particles()));

  gc.draw_string(210.0f, bottom_line, "Springs: ");
  gc.draw_string(280.0f, bottom_line, to_string(world.get_num_springs()));

  gc.draw_string(410.0f, bottom_line, "Zoom: ");
  gc.draw_string(480.0f, bottom_line, to_string(WorldViewComponent::instance()->get_zoom()));

  gc.draw_string (610, bottom_line, "..:: Construo V" VERSION " ::..");
  //gc.draw_string (680, bottom_line, to_string(WorldViewComponent::instance()->get_zoom()));
}

/* EOF */
