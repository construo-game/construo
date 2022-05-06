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

#include <assert.h>
#include "construo.hpp"
#include "zoom_graphic_context.hpp"
#include "gui_file_manager.hpp"
#include "screen_manager.hpp"
#include "gui_buttons.hpp"

GUIFileManager* GUIFileManager::instance_ = nullptr;

GUIFileManager::GUIFileManager(Mode m) :
  GUIChildManager(),
  m_mode(m),
  m_btn_up_directory(),
  m_btn_close(),
  m_btn_scroll_up(),
  m_btn_scroll_down(),
  m_btn_update_directory(),
  m_directory()
{
  instance_ = this;

  if (m_mode == SAVE_MANAGER) {
    m_directory = create<GUIDirectory>("/", GUIDirectory::SAVE_DIRECTORY);
  } else {
    m_directory = create<GUIDirectory>("/", GUIDirectory::LOAD_DIRECTORY);
  }

  m_btn_up_directory = create<GUIGenericButton>("Up", [this]{
    directory_up();
  });

  m_btn_close = create<GUIGenericButton>("Close", []{
    ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
  });

  m_btn_scroll_up = create<GUIGenericButton>("/\\", [this]{
    scroll_up();
  });

  m_btn_scroll_down = create<GUIGenericButton>("\\/", [this]{
    scroll_down();
  });

  m_btn_update_directory = create<GUIGenericButton>("Reload", [this]{
    update_current_directory();
  });
}

GUIFileManager::~GUIFileManager ()
{
}

void
GUIFileManager::set_geometry(float x, float y, float width, float height)
{
  GUIChildManager::set_geometry(x, y, width, height);

  m_btn_up_directory->set_geometry(0.0f, 0.0f, 100.0f, 25.0f);
  m_btn_close->set_geometry(width - 100.0f, 0.0f, 100.0f, 25.0f);

  m_btn_scroll_up->set_geometry(width - 30.0f, height / 2.0f - 100.0f, 25.0f, 50.0f);
  m_btn_scroll_down->set_geometry(width - 30.0f, height / 2.0f, 25.0f, 50.0f);

  m_btn_update_directory->set_geometry(width - 150.0f, height - 25.0f, 150.0f, 25.0f);

  m_directory->set_geometry(x, y, width, height);
}

void
GUIFileManager::open_directory (const std::string& pathname)
{
  GUIDirectory* old_directory = m_directory;
  std::unique_ptr<GUIDirectory> new_directory;
  if (m_mode == SAVE_MANAGER) {
    m_directory = create<GUIDirectory>(pathname, GUIDirectory::SAVE_DIRECTORY);
  } else {
    m_directory = create<GUIDirectory>(pathname, GUIDirectory::LOAD_DIRECTORY);
  }
  m_directory->set_geometry(m_x, m_y, m_width, m_height);

  remove(old_directory);
}

void
GUIFileManager::directory_up()
{
  std::string pathname = m_directory->get_path ();

  // FIXME: UGLY code
  if (pathname == "/")
    {
      // already at the top most directory
      return;
    }
  else
    {
      assert(*(pathname.end()-1) == '/');

      for (int i = static_cast<int>(pathname.size()) - 2; i >= 0; --i)
        {
          if (pathname[i] == '/') // Found second '/'
            {
              pathname = pathname.substr(0, i+1);
              break;
            }
        }

      std::cout << "Directory Up: " << m_directory->get_path () << " -> " << pathname << std::endl;
      open_directory (pathname);
    }
}

void
GUIFileManager::draw_overlay(GraphicContext& gc)
{
  gc.draw_string(200, 16, m_directory->get_path());
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
