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

#include "world_gui_manager.hpp"

#include <fmt/format.h>
#include <geom/rect.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>

#include "controller.hpp"
#include "construo.hpp"
#include "graphic_context.hpp"
#include "worldview_component.hpp"
#include "worldview_insert_tool.hpp"
#include "gui_button.hpp"
#include "gui_window.hpp"
#include "gui_label.hpp"
#include "screen_manager.hpp"
#include "root_graphic_context.hpp"

WorldGUIManager* WorldGUIManager::instance_ = nullptr;

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
  m_zoomin_button(),
  m_particle_mass_label(),
  m_particle_mass_increase_button(),
  m_particle_mass_decrease_button(),
  m_last_geometry()
{
  instance_  = this;

  m_worldview_component = create<WorldViewComponent>();

  // simulation
  m_run_button = create<GUIButton>(
    "Run",
    []{ Controller::instance()->start_simulation(); },
    []{ return Controller::instance()->is_running (); });

  m_slowmo_button = create<GUIButton>(
    "SlowMotion",
    []{ Controller::instance()->set_slow_down(!Controller::instance()->slow_down_active ()); },
    []{ return Controller::instance()->slow_down_active(); });

  m_load_button = create<GUIButton>(
    "Load",
    []{ ScreenManager::instance()->set_gui(ScreenManager::LOAD_GUI); });

  m_save_button = create<GUIButton>(
    "Save",
    []{ ScreenManager::instance()->set_gui(ScreenManager::SAVE_GUI); });

  m_undo_button = create<GUIButton>(
    "Undo",
    []{ return Controller::instance()->undo(); });
  m_redo_button = create<GUIButton>(
    "Redo",
    []{ return Controller::instance()->redo(); });

  m_actioncam_button = create<GUIButton>(
    "ActionCam",
    []{ Controller::instance()->set_action_cam(!Controller::instance()->get_action_cam()); },
    []{ return Controller::instance()->get_action_cam(); });

  m_dots_button = create<GUIButton>(
    "Hide Dots",
    []{ Controller::instance()->set_hide_dots(!Controller::instance()->get_hide_dots()); },
    []{ return Controller::instance()->get_hide_dots(); });

  m_grid_button = create<GUIButton>(
    "Use Grid",
    [this]{ m_worldview_component->on_grid_press(geom::fpoint(0, 0)); },
    [this]{ return m_worldview_component->uses_grid(); });

  m_quit_button = create<GUIButton>("Quit", []{
    ScreenManager::instance()->quit();
  });

  // toolbar
  m_insert_button = create<GUIButton>(
    "Insert",
    [this]{ m_worldview_component->set_mode (WorldViewComponent::INSERT_MODE); },
    [this]{ return m_worldview_component->get_mode() == WorldViewComponent::INSERT_MODE; });

  m_select_button = create<GUIButton>(
    "Select",
    [this]{ m_worldview_component->set_mode (WorldViewComponent::SELECT_MODE); },
    [this]{ return m_worldview_component->get_mode() == WorldViewComponent::SELECT_MODE; });

  m_collider_button = create<GUIButton>(
    "Collider",
    [this]{ m_worldview_component->set_mode (WorldViewComponent::COLLIDER_MODE); },
    [this]{ return m_worldview_component->get_mode() == WorldViewComponent::COLLIDER_MODE; });

  m_zoom_button = create<GUIButton>(
    "Zoom",
    [this]{ m_worldview_component->set_mode (WorldViewComponent::ZOOM_MODE); },
    [this]{ return m_worldview_component->get_mode() == WorldViewComponent::ZOOM_MODE; });

  m_zoomout_button = create<GUIButton>(
    "-",
    [this]{ m_worldview_component->wheel_up(geom::fpoint(g_graphic_context->get_width()/2,
                                                         g_graphic_context->get_height()/2)); });
  m_zoomin_button = create<GUIButton>(
    "+",
    [this]{ m_worldview_component->wheel_down(geom::fpoint(g_graphic_context->get_width()/2,
                                                           g_graphic_context->get_height()/2)); });

  // particle mass
  m_particle_mass_label = create<GUILabel>([]{
    return fmt::format("Mass: {:.1f}", Controller::instance()->get_particle_mass());
  });
  m_particle_mass_increase_button = create<GUIButton>("+", [](){
    Controller::instance()->set_particle_mass(std::max(0.1f,
                                                       Math::round_to_float(Controller::instance()->get_particle_mass() + 0.1f, 0.1f)));
  });
  m_particle_mass_decrease_button = create<GUIButton>("-", [](){
    Controller::instance()->set_particle_mass(std::max(0.1f,
                                                       Math::round_to_float(Controller::instance()->get_particle_mass() - 0.1f, 0.1f)));
  });

  //create<GUIWindow>("Test Window",   200, 100, 200, 90);

  /*
    GUIWindow* window = new GUIWindow ("Window Title", 300, 100, 300, 400);
    window->add (new GUIButton ("Testbutton", 10, 10, 100, 25);
    add (window);
  */
}

WorldGUIManager::~WorldGUIManager ()
{
}

void
WorldGUIManager::set_geometry(geom::frect const& geometry)
{
  // FIXME: This doesn't work when the window goes from maximized to
  // move (click+drag on titlebar)
  if (geometry.size() != m_last_geometry.size()) {
    // Do clever enlarging of the viewport only when the window is
    // resized, but avoid it when the window is moved
    auto const offset = m_last_geometry.topleft().as_vec() - geometry.topleft().as_vec();
    m_worldview_component->zoom().set_offset(
      geom::foffset(m_worldview_component->zoom().get_x_offset() + offset.x / m_worldview_component->zoom().get_scale(),
                    m_worldview_component->zoom().get_y_offset() + offset.y / m_worldview_component->zoom().get_scale()));
  }

  GUIManager::set_geometry(geometry);

  geom::fsize const button_size(75.0f, 25.0f);
  auto btn_left = [=](int row) {
    return geom::frect(geom::fpoint(10.0f, 80.0f + static_cast<float>(row) * 30.0f),
                       button_size);
  };
  auto btn_right = [=](int row) {
    return geom::frect(geom::fpoint(geometry.width() - button_size.width() - 10.0f,
                                    50.0f + static_cast<float>(row) * 30.0f),
                       button_size);
  };
  auto btn_split = [](geom::frect const& rect, int col) {
    return geom::frect(rect.left() + (rect.width()/2.0f) * static_cast<float>(col) + 3,
                       rect.top(),
                       rect.left() + (rect.width()/2.0f) * static_cast<float>(col + 1) - 3,
                       rect.bottom());
  };

  m_worldview_component->set_geometry(geom::frect(geometry.size()));

  m_run_button->set_geometry(btn_left(0));
  m_slowmo_button->set_geometry(btn_left(1));
  m_load_button->set_geometry(btn_left(9));
  m_save_button->set_geometry(btn_left(10));

  m_undo_button->set_geometry(btn_left(6));
  m_redo_button->set_geometry(btn_left(7));

  m_actioncam_button->set_geometry(btn_left(2));
  m_dots_button->set_geometry(btn_left(3));
  m_grid_button->set_geometry(btn_left(4));
  m_quit_button->set_geometry(btn_left(12));

  m_insert_button->set_geometry(btn_right(4));
  m_select_button->set_geometry(btn_right(5));
  m_collider_button->set_geometry(btn_right(6));
  m_zoom_button->set_geometry(btn_right(7));

  m_zoomin_button->set_geometry(btn_split(btn_right(8), 0));
  m_zoomout_button->set_geometry(btn_split(btn_right(8), 1));

  // particle mass
  m_particle_mass_label->set_geometry(btn_right(10));
  m_particle_mass_increase_button->set_geometry(btn_split(btn_right(11), 0));
  m_particle_mass_decrease_button->set_geometry(btn_split(btn_right(11), 1));

  m_last_geometry = geometry;
}

void
WorldGUIManager::update()
{
  Controller::instance()->update();
}

void
WorldGUIManager::draw_overlay(GraphicContext& gc)
{
  int column = 0;
  int row = 0;
  auto pos = [&](){
    return geom::fpoint(10.0f + 200.0f * static_cast<float>(column),
                        20.0f + 12.0f * static_cast<float>(row++));
  };

  gc.draw_string(pos(), "      [1-9] - quick save");
  gc.draw_string(pos(), "[shift 1-9] - quick load");
  gc.draw_string(pos(), "    [space] - run simulation");
  gc.draw_string(pos(), "      [tab] - toggle slow motion");
  gc.draw_string(pos(), "      [F11] - toggle fullscreen");

  column += 1; row = 0;
  gc.draw_string(pos(), "     [c] - clear scene");
  gc.draw_string(pos(), "     [u] - undo to last state");
  gc.draw_string(pos(), "     [r] - redo (undo an undo)");
  gc.draw_string(pos(), "   [+/-] - zoom in/out");
  gc.draw_string(pos(), "     [g] - toggle grid");

  column += 2; row = 0;

  switch (m_worldview_component->get_mode())
    {
    case WorldViewComponent::INSERT_MODE:
      gc.draw_string(pos(), "  [left] - insert/connect spots");
      gc.draw_string(pos(), "[middle] - scroll");
      gc.draw_string(pos(), " [right] - remove spot");
      column -= 1;
      gc.draw_string(pos(), "     [f] - fix current dot");
      break;

    case WorldViewComponent::SELECT_MODE:
      gc.draw_string(pos(), "  [left] - create/move selection");
      gc.draw_string(pos(), "[middle] - scroll");
      gc.draw_string(pos(), " [right] - rotate selection");

      column -= 1;
      gc.draw_string(pos(), "     [v] - set velocity");
      gc.draw_string(pos(), "     [d] - duplicate selection");
      gc.draw_string(pos(), "     [h] - flip selection");
      gc.draw_string(pos(), "     [f] - fix selection");
      gc.draw_string(pos(), "     [j] - join dots");
      gc.draw_string(pos(), "     [s] - scale selection");
      break;

    case WorldViewComponent::ZOOM_MODE:
      gc.draw_string(pos(), "  [left] - zoom into region");
      gc.draw_string(pos(), "[middle] - scroll");
      gc.draw_string(pos(), " [right] - zoom out");
      break;

    case WorldViewComponent::COLLIDER_MODE:
      gc.draw_string(pos(), "  [left] - create/move collider");
      gc.draw_string(pos(), "[middle] - scroll");
      gc.draw_string(pos(), " [right] - remove collider");
      break;

    default:
      break;
    }

  World& world = Controller::instance()->get_world ();

  float const bottom_line = gc.get_height() - 10;
  gc.draw_string(geom::fpoint(10.0f, bottom_line - 20.0f), "FPS: ");
  gc.draw_string(geom::fpoint(80.0f, bottom_line - 20.0f), fmt::format("{:6.2f}", get_fps()));

  gc.draw_string(geom::fpoint(10.0f, bottom_line), "Pos: ");
  auto const& p = m_worldview_component->zoom().screen_to_world(g_input_context->get_mouse_pos());
  gc.draw_string(geom::fpoint(80.0f, bottom_line), fmt::format("{:6.2f} {:6.2f}", p.x(), p.y()));

  gc.draw_string(geom::fpoint(210.0f, bottom_line - 20.0f), "Particles: ");
  gc.draw_string(geom::fpoint(280.0f, bottom_line - 20.0f), std::to_string(world.particles().size()));

  gc.draw_string(geom::fpoint(210.0f, bottom_line), "Springs: ");
  gc.draw_string(geom::fpoint(280.0f, bottom_line), std::to_string(world.springs().size()));

  gc.draw_string(geom::fpoint(410.0f, bottom_line), "Zoom: ");
  gc.draw_string(geom::fpoint(480.0f, bottom_line), fmt::format("{:5.2f}", m_worldview_component->get_scale()));

  gc.draw_string(geom::fpoint(610, bottom_line), "Construo " VERSION);
  //gc.draw_string(680, bottom_line, std::to_string(m_worldview_component->get_scale()));
}

/* EOF */
