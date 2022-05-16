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

#include "gui_new_file_button.hpp"

#include <time.h>

#include <logmich/log.hpp>

#include "controller.hpp"
#include "screen_manager.hpp"
#include "path.hpp"

GUINewFileButton::GUINewFileButton(const std::string& filename) :
  GUIFileButton(filename)
{
}

void
GUINewFileButton::draw(GraphicContext& parent_gc)
{
  parent_gc.draw_fill_rect(m_geometry, Color (0x999900FF));

  parent_gc.draw_string(m_geometry.topleft() + geom::foffset(20, 75), "..:: Save to new file ::..");

  if (m_mouse_over) {
    parent_gc.draw_rect(m_geometry, Color (0xFFFFFFFF));
  } else {
    parent_gc.draw_rect(m_geometry, Color (0xFF0000FF));
  }
}

std::string
GUINewFileButton::generate_filename()
{
  char buffer[32];
  time_t curtime;
  struct tm *loctime;
  curtime = time (NULL);
  loctime = localtime(&curtime);
  strftime(buffer, 32, "%Y%m%d-%H%M%S", loctime);

  return path_join(m_path, std::string(buffer) + ".construo");
}

void
GUINewFileButton::on_click()
{
  std::string filename = generate_filename();
  log_info("Saving to: {}", filename);
  Controller::instance()->save_world (filename);
  ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
}

/* EOF */
