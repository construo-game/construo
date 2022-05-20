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

#ifndef HEADER_WORLD_GUI_MANAGER_HPP
#define HEADER_WORLD_GUI_MANAGER_HPP

#include "gui_manager.hpp"

#include <geom/rect.hpp>

namespace construo {

class WorldGUIManager : public GUIManager
{
private:
  static WorldGUIManager* instance_;
public:
  static WorldGUIManager* instance() { return instance_; }

  WorldGUIManager ();
  virtual ~WorldGUIManager ();

  void set_geometry(geom::frect const& geometry) override;

  void update() override;
  void draw_overlay(GraphicContext& gc) override;

private:
  WorldViewWidget* m_worldview_widget;

  GUIWidget* m_run_button;
  GUIWidget* m_slowmo_button;
  GUIWidget* m_load_button;
  GUIWidget* m_save_button;

  GUIWidget* m_undo_button;
  GUIWidget* m_redo_button;

  GUIWidget* m_actioncam_button;
  GUIWidget* m_dots_button;
  GUIWidget* m_grid_button;
  GUIWidget* m_quit_button;

  GUIWidget* m_insert_button;
  GUIWidget* m_select_button;
  GUIWidget* m_collider_button;
  GUIWidget* m_zoom_button;

  GUIWidget* m_zoomout_button;
  GUIWidget* m_zoomin_button;

  GUIWidget* m_particle_mass_label;
  GUIWidget* m_particle_mass_increase_button;
  GUIWidget* m_particle_mass_decrease_button;

  GUIWidget* m_spring_stiffness_label;
  GUIWidget* m_spring_stiffness_increase_button;
  GUIWidget* m_spring_stiffness_decrease_button;

  geom::frect m_last_geometry;

public:
  WorldGUIManager(const WorldGUIManager&) = delete;
  WorldGUIManager& operator=(const WorldGUIManager&) = delete;
};

} // namespace construo

#endif

/* EOF */
