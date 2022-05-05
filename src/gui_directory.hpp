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

#ifndef HEADER_CONSTRUO_GUI_DIRECTORY_HPP
#define HEADER_CONSTRUO_GUI_DIRECTORY_HPP

#include <vector>
#include "gui_child_manager.hpp"

class ZoomGraphicContext;
class GUIFileButton;

/** Widget which shows a complete directory */
class GUIDirectory : public GUIChildManager
{
public:
  enum Mode { LOAD_DIRECTORY, SAVE_DIRECTORY };

public:
  GUIDirectory(const std::string& pathname, Mode m);
  ~GUIDirectory();

  void set_geometry(float x, float y, float width, float height) override;

  void wheel_up(float x, float y) override;
  void wheel_down(float x, float y) override;

  std::string get_path() const { return m_pathname; }

  void move_up();
  void move_down();

private:
  void place_components();

private:
  std::string m_pathname;
  std::vector<GUIFileButton*> m_files;
  int m_last_row;
  int m_row_offset;
  unsigned long m_mtime;
  Mode m_mode;

public:
  GUIDirectory(const GUIDirectory&) = delete;
  GUIDirectory& operator=(const GUIDirectory&) = delete;
};

#endif

/* EOF */
