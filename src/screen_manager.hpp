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

#ifndef HEADER_CONSTRUO_SCREEN_MANAGER_HPP
#define HEADER_CONSTRUO_SCREEN_MANAGER_HPP

#include <memory>

#include "fwd.hpp"

class ScreenManager
{
private:
  static std::unique_ptr<ScreenManager> s_instance;

public:
  static ScreenManager* instance ();

public:
  enum { LOAD_GUI, SAVE_GUI, WORLD_GUI };

private:
  ScreenManager ();

public:
  void set_gui (int gui_id);
  void quit();
  bool is_finished();
  void run_once(GraphicContext& gc);
  void draw(GraphicContext& gc);
  void resize(float width, float height);

private:
  bool m_do_quit;

  std::unique_ptr<GUIManager> m_load_gui_manager;
  std::unique_ptr<GUIManager> m_save_gui_manager;
  std::unique_ptr<GUIManager> m_world_gui_manager;

  GUIManager* m_current_gui_manager;

public:
  ScreenManager(const ScreenManager&) = delete;
  ScreenManager& operator=(const ScreenManager&) = delete;
};

#endif

/* EOF */
