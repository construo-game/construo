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

#include "gui_directory.hpp"

#include <logmich/log.hpp>

#include "construo.hpp"
#include "gui_directory_button.hpp"
#include "gui_file_manager.hpp"
#include "gui_new_file_button.hpp"
#include "path.hpp"
#include "system_context.hpp"
#include "world.hpp"
#include "world_button.hpp"
#include "worldview_collider_tool.hpp"

namespace construo {

GUIDirectory::GUIDirectory(GUIFileManager& file_manager, const std::string& pathname, Mode mode) :
  WidgetGroup(),
  m_file_manager(file_manager),
  m_pathname(pathname),
  m_mode(mode),
  m_world_cache(),
  m_items(),
  m_last_row(0),
  m_row_offset(0)
{
  read_directory();
  place_widgets();
}

GUIDirectory::~GUIDirectory ()
{
}

void
GUIDirectory::read_directory()
{
  m_items.clear();

  if (m_mode == SAVE_DIRECTORY && m_pathname != "/") {
    m_items.emplace_back([this]{ return std::make_unique<GUINewFileButton>(m_pathname); });
  }

  for (auto const& entry : g_system_context->read_directory(m_pathname))
  {
    std::string const filename = path_join(m_pathname, entry);
    FileType const type = g_system_context->get_file_type(filename);

    if (type == FT_DIRECTORY)
    {
      m_items.emplace_back([this, filename]{
        return std::make_unique<GUIDirectoryButton>(filename, [this, filename]{
          m_file_manager.open_directory(filename);
        });
      });
    }
    else if (type == FT_CONSTRUO_FILE)
    {
      if (m_mode == SAVE_DIRECTORY) {
        m_items.emplace_back([this, filename]{
          return std::make_unique<WorldButton>(m_world_cache, filename, WorldButton::SAVE_BUTTON);
        });
      } else {
        m_items.emplace_back([this, filename]{
          return std::make_unique<WorldButton>(m_world_cache, filename, WorldButton::LOAD_BUTTON);
        });
      }
    }
    else // (type == FT_UNKNOWN_FILE)
    {
      // ignore unknown files
      log_debug("GUIFileManager: ignoring '{}' since it has unknown filetype", filename);
    }
  }
}

void
GUIDirectory::place_widgets()
{
  if (m_items.empty()) { return; }

  // Remove all file widgets
  clear();

  int const columns = static_cast<int>(m_geometry.width() / 320.0f) + 1;
  int const rows = static_cast<int>(m_geometry.height() / 240.0f) + 1;

  // calculate how far down scrolling is allowed
  m_last_row = std::max((((static_cast<int>(m_items.size()) + columns - 1) + columns) / columns - 1) - rows, 0);

  // clamp scroll offset to new limits
  m_row_offset = std::clamp(m_row_offset, 0, m_last_row);

  float const spacing = 40.0f;
  float const padding = 50.0f;
  geom::fsize const thumb_size((m_geometry.width() - (2.0f * padding) - (spacing * static_cast<float>(columns - 1))) / static_cast<float>(columns),
                               (m_geometry.height() - (2.0f * padding) - (spacing * static_cast<float>(rows - 1))) / static_cast<float>(rows));

  int row = 0;
  int column = 0;
  int count = 0;
  for(std::vector<GUIFileButton*>::size_type i = m_row_offset * columns;
      i < m_items.size() && count < (columns * rows);
      ++i)
  {
    std::unique_ptr<GUIWidget> comp = m_items[i]();
    comp->set_geometry(geom::frect(geom::fpoint(static_cast<float>(column) * (thumb_size.width() + spacing) + padding,
                                                static_cast<float>(row) * (thumb_size.height() + spacing) + padding),
                                   thumb_size));
    add(std::move(comp));

    column += 1;
    if (column >= columns) { // row is full
      column = 0;
      row += 1;
    }
    ++count;
  }
}

void
GUIDirectory::set_geometry(geom::frect const& geometry)
{
  WidgetGroup::set_geometry(geometry);
  place_widgets();
}

void
GUIDirectory::move_up ()
{
  m_row_offset = std::clamp(m_row_offset - 1, 0, m_last_row);
  place_widgets ();
}

void
GUIDirectory::move_down ()
{
  m_row_offset = std::clamp(m_row_offset + 1, 0, m_last_row);
  place_widgets ();
}

void
GUIDirectory::wheel_up (geom::fpoint const& pos)
{
  move_up();
}

void
GUIDirectory::wheel_down (geom::fpoint const& pos)
{
  move_down();
}

} // namespace construo

/* EOF */
