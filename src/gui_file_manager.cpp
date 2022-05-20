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

#include "gui_file_manager.hpp"

#include <assert.h>

#include "construo.hpp"
#include "gui_button.hpp"
#include "path.hpp"
#include "screen_manager.hpp"
#include "zoom_graphic_context.hpp"

GUIFileManager::GUIFileManager(Mode m) :
  WidgetGroup(),
  m_mode(m),
  m_btn_up_directory(),
  m_btn_close(),
  m_btn_scroll_up(),
  m_btn_scroll_down(),
  m_btn_update_directory(),
  m_directory()
{
  m_btn_up_directory = create<GUIButton>("Up", [this]{
    directory_up();
  });

  m_btn_close = create<GUIButton>("Close", []{
    ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
  });

  m_btn_scroll_up = create<GUIButton>("/\\", [this]{
    scroll_up();
  });

  m_btn_scroll_down = create<GUIButton>("\\/", [this]{
    scroll_down();
  });

  m_btn_update_directory = create<GUIButton>("Reload", [this]{
    update_current_directory();
  });

  if (m_mode == SAVE_MANAGER) {
    m_directory = create<GUIDirectory>(*this, "/", GUIDirectory::SAVE_DIRECTORY);
  } else {
    m_directory = create<GUIDirectory>(*this, "/", GUIDirectory::LOAD_DIRECTORY);
  }
}

GUIFileManager::~GUIFileManager ()
{
}

void
GUIFileManager::set_geometry(geom::frect const& geometry)
{
  WidgetGroup::set_geometry(geometry);

  geom::fsize const btn_size(100.0f, 25.0f);
  m_btn_up_directory->set_geometry(geom::frect(btn_size));
  m_btn_close->set_geometry(geom::frect(geom::fpoint(m_geometry.width() - 100.0f, 0.0f), btn_size));

  m_btn_scroll_up->set_geometry(geom::frect(geom::fpoint(m_geometry.width() - 30.0f, m_geometry.height() / 2.0f - 100.0f),
                                            geom::fsize(25.0f, 50.0f)));
  m_btn_scroll_down->set_geometry(geom::frect(geom::fpoint(m_geometry.width() - 30.0f, m_geometry.height() / 2.0f),
                                              geom::fsize(25.0f, 50.0f)));

  m_btn_update_directory->set_geometry(geom::frect(geom::fpoint(m_geometry.width() - 150.0f, m_geometry.height() - 25.0f),
                                                   geom::fsize(150.0f, 25.0f)));

  m_directory->set_geometry(geometry);
}

void
GUIFileManager::open_directory (const std::string& pathname)
{
  GUIDirectory* old_directory = m_directory;
  if (m_mode == SAVE_MANAGER) {
    m_directory = create<GUIDirectory>(*this, pathname, GUIDirectory::SAVE_DIRECTORY);
  } else {
    m_directory = create<GUIDirectory>(*this, pathname, GUIDirectory::LOAD_DIRECTORY);
  }
  m_directory->set_geometry(m_geometry);

  remove(old_directory);
}

void
GUIFileManager::directory_up()
{
  std::string const path = m_directory->get_path();
  open_directory(path_dirname(path));
}

void
GUIFileManager::draw_overlay(GraphicContext& gc)
{
  gc.draw_string(geom::fpoint(200.0f, 16.0f), m_directory->get_path());
}

void
GUIFileManager::scroll_up ()
{
  m_directory->move_up();
}

void
GUIFileManager::scroll_down ()
{
  m_directory->move_down();
}

void
GUIFileManager::update_current_directory()
{
  open_directory(m_directory->get_path());
}

/* EOF */
