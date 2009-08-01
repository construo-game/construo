//  $Id$
//
//  Construo - A wire-frame construction gamee
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "load_gui_manager.hpp"
#include "save_gui_manager.hpp"
#include "world_gui_manager.hpp"
#include "screen_manager.hpp"

ScreenManager* ScreenManager::instance_ = 0;

ScreenManager::ScreenManager ()
  : do_quit(false)
{
  load_gui_manager  = new LoadGUIManager();
  save_gui_manager  = new SaveGUIManager();
  world_gui_manager = new WorldGUIManager();

  current_gui_manager = world_gui_manager;
}

void
ScreenManager::run_once ()
{
  current_gui_manager->run_once ();
}

bool
ScreenManager::is_finished ()
{
  return do_quit;
}

void
ScreenManager::quit()
{
  do_quit = true;
}

void
ScreenManager::set_gui (int gui_id)
{
  switch (gui_id)
    {
    case WORLD_GUI:
      current_gui_manager = world_gui_manager;
      break;
    case LOAD_GUI:
      current_gui_manager = load_gui_manager;
      break;
    case SAVE_GUI:
      current_gui_manager = save_gui_manager;
      break;
    }
}

ScreenManager*
ScreenManager::instance ()
{
  if (instance_ == 0)
    return (instance_ = new ScreenManager ());
  else
    return instance_;
}

/* EOF */
