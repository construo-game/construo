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

#include "load_gui_manager.hpp"

#include <geom/rect.hpp>

#include "gui_file_manager.hpp"
#include "construo.hpp"
#include "system_context.hpp"
#include "root_graphic_context.hpp"

namespace construo {

LoadGUIManager::LoadGUIManager() :
  m_file_manager()
{
  m_file_manager = create<GUIFileManager>(GUIFileManager::LOAD_MANAGER);
}

void
LoadGUIManager::set_geometry(geom::frect const& geometry)
{
  GUIManager::set_geometry(geometry);
  m_file_manager->set_geometry(geom::frect(geometry.size()));
}

void
LoadGUIManager::draw_overlay(GraphicContext& gc)
{
  gc.draw_string(geom::fpoint(10.0f, gc.geometry().height() - 10.0f),
                 "Load Dialog");
}

void
LoadGUIManager::run_once(GraphicContext& gc)
{
  GUIManager::run_once(gc);
  g_system_context->sleep(1000);
}

} // namespace construo

/* EOF */
