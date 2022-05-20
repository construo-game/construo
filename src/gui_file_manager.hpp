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

#ifndef HEADER_CONSTRUO_GUI_FILE_MANAGER_HPP
#define HEADER_CONSTRUO_GUI_FILE_MANAGER_HPP

#include <map>
#include <string>

#include "fwd.hpp"
#include "widget_group.hpp"
#include "gui_directory.hpp"

class GUIFileManager : public WidgetGroup
{
public:
  enum Mode { LOAD_MANAGER, SAVE_MANAGER };

public:
  GUIFileManager(Mode m);
  ~GUIFileManager();

  void draw_overlay(GraphicContext& gc) override;
  void set_geometry(geom::frect const& geometry) override;

  /** Switch the GUIFileManagers view to the directory given by
      pathname */
  void open_directory(const std::string& pathname);

  /** move one directory up, aka cut the last directory from a
      directory pathname */
  void directory_up();

  void update_current_directory();

  void scroll_up() override;
  void scroll_down() override;

private:
  Mode m_mode;

  GUIButton* m_btn_up_directory;
  GUIButton* m_btn_close;
  GUIButton* m_btn_scroll_up;
  GUIButton* m_btn_scroll_down;
  GUIButton* m_btn_update_directory;

  GUIDirectory* m_directory;

public:
  GUIFileManager(const GUIFileManager&) = delete;
  GUIFileManager& operator=(const GUIFileManager&) = delete;
};

#endif

/* EOF */
