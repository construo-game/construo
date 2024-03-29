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

#include <functional>
#include <vector>

#include "fwd.hpp"
#include "widget_group.hpp"
#include "world_cache.hpp"

/** Widget which shows a complete directory */
namespace construo {

class GUIDirectory : public WidgetGroup
{
public:
  enum Mode { LOAD_DIRECTORY, SAVE_DIRECTORY };

public:
  GUIDirectory(GUIFileManager& file_manager, const std::string& pathname, Mode mode);
  ~GUIDirectory();

  void set_geometry(geom::frect const& geometry) override;

  void wheel_up(geom::fpoint const& pos) override;
  void wheel_down(geom::fpoint const& pos) override;

  std::string get_path() const { return m_pathname; }

  void move_up();
  void move_down();

private:
  void read_directory();
  void place_widgets();

private:
  GUIFileManager& m_file_manager;
  std::string m_pathname;
  Mode m_mode;
  WorldCache m_world_cache;
  std::vector<std::function<std::unique_ptr<GUIWidget>()>> m_items;
  int m_last_row;
  int m_row_offset;

public:
  GUIDirectory(const GUIDirectory&) = delete;
  GUIDirectory& operator=(const GUIDirectory&) = delete;
};

} // namespace construo

#endif

/* EOF */
