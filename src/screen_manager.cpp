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
#include "save_gui_manager.hpp"
#include "world_gui_manager.hpp"
#include "screen_manager.hpp"

std::unique_ptr<ScreenManager> ScreenManager::s_instance;

ScreenManager::ScreenManager() :
  m_do_quit(false),
  m_load_gui_manager(std::make_unique<LoadGUIManager>()),
  m_save_gui_manager(std::make_unique<SaveGUIManager>()),
  m_world_gui_manager(std::make_unique<WorldGUIManager>()),
  m_current_gui_manager(m_world_gui_manager.get())
{
}

void
ScreenManager::run_once(GraphicContext& gc)
{
  m_current_gui_manager->run_once(gc);
}

void
ScreenManager::draw(GraphicContext& gc)
{
  m_current_gui_manager->draw(gc);
}

bool
ScreenManager::is_finished ()
{
  return m_do_quit;
}

void
ScreenManager::quit()
{
  m_do_quit = true;
}

void
ScreenManager::set_gui(int gui_id)
{
  switch (gui_id)
  {
    case WORLD_GUI:
      m_current_gui_manager = m_world_gui_manager.get();
      break;
    case LOAD_GUI:
      m_current_gui_manager = m_load_gui_manager.get();
      break;
    case SAVE_GUI:
      m_current_gui_manager = m_save_gui_manager.get();
      break;
  }
}

void
ScreenManager::resize(float width, float height)
{
  m_load_gui_manager->resize(width, height);
  m_save_gui_manager->resize(width, height);
  m_world_gui_manager->resize(width, height);
}

ScreenManager*
ScreenManager::instance ()
{
  if (!s_instance) {
    s_instance.reset(new ScreenManager);
  }

  return s_instance.get();
}

/* EOF */
