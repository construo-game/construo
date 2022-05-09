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

#ifndef HEADER_CONSTRUO_LOAD_GUI_MANAGER_HPP
#define HEADER_CONSTRUO_LOAD_GUI_MANAGER_HPP

#include "gui_manager.hpp"

class LoadGUIManager : public GUIManager
{
public:
  LoadGUIManager ();

  void resize(float width, float height) override;
  void draw_overlay(GraphicContext& gc) override;
  void run_once(GraphicContext& gc) override;

private:
  GUIFileManager* m_file_manager;

public:
  LoadGUIManager(const LoadGUIManager&) = delete;
  LoadGUIManager& operator=(const LoadGUIManager&) = delete;
};

#endif

/* EOF */
