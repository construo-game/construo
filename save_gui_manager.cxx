//  $Id$
//
//  Pingus - A free Lemmings clone
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#include "gui_file_manager.hxx"
#include "construo.hxx"
#include "system_context.hxx"
#include "save_gui_manager.hxx"

SaveGUIManager::SaveGUIManager ()
{
  file_manager = new GUIFileManager (GUIFileManager::SAVE_MANAGER);
  add (file_manager);
}

void
SaveGUIManager::draw_overlay ()
{
  graphic_context->draw_string(10, graphic_context->get_height() - 10,
                               "Save Dialog");
}

void
SaveGUIManager::run_once ()
{
  GUIFileManager::set_instance(file_manager);
  GUIManager::run_once();
  system_context->sleep (1000);
}

/* EOF */
