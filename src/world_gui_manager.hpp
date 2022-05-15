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
  WorldViewComponent* m_worldview_component;

  GUIComponent* m_run_button;
  GUIComponent* m_slowmo_button;
  GUIComponent* m_load_button;
  GUIComponent* m_save_button;

  GUIComponent* m_undo_button;
  GUIComponent* m_redo_button;

  GUIComponent* m_actioncam_button;
  GUIComponent* m_dots_button;
  GUIComponent* m_grid_button;
  GUIComponent* m_quit_button;

  GUIComponent* m_insert_button;
  GUIComponent* m_select_button;
  GUIComponent* m_collider_button;
  GUIComponent* m_zoom_button;

  GUIComponent* m_zoomout_button;
  GUIComponent* m_zoomin_button;

  GUIComponent* m_particle_mass_label;
  GUIComponent* m_particle_mass_increase_button;
  GUIComponent* m_particle_mass_decrease_button;

  geom::frect m_last_geometry;

public:
  WorldGUIManager(const WorldGUIManager&) = delete;
  WorldGUIManager& operator=(const WorldGUIManager&) = delete;
};

#endif

/* EOF */
