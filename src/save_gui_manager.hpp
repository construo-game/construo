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

#ifndef HEADER_CONSTRUO_SAVE_GUI_MANAGER_HPP
#define HEADER_CONSTRUO_SAVE_GUI_MANAGER_HPP

#include "gui_manager.hpp"

namespace construo {

class SaveGUIManager : public GUIManager
{
public:
  SaveGUIManager ();

  void set_geometry(geom::frect const& geometry) override;
  void draw_overlay(GraphicContext& gc) override;
  void run_once(GraphicContext& gc) override;

private:
  GUIFileManager* m_file_manager;

public:
  SaveGUIManager(const SaveGUIManager&) = delete;
  SaveGUIManager& operator=(const SaveGUIManager&) = delete;
};

} // namespace construo

#endif

/* EOF */
