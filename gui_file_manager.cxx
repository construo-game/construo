//  $Id$
//
//  Construo - A wire-frame construction gamee
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

#include "construo.hxx"
#include "zoom_graphic_context.hxx"
#include "gui_file_manager.hxx"
#include "screen_manager.hxx"
#include "gui_buttons.hxx"

void DirectoryUp ()
{
  GUIFileManager::instance()->directory_up();
}

void CloseFileManager ()
{
  ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
}

GUIFileManager* GUIFileManager::instance_ = 0;

GUIFileManager::GUIFileManager ()
  : GUIChildManager (0, 0, 800, 600)
{
  current_directory = new GUIDirectory ("/");
  directories["/"] = current_directory;
  
  add (new GUIGenericButton("Up", 0,0, 100, 25, DirectoryUp));
  add (new GUIGenericButton("Close", 700, 0, 100, 25, CloseFileManager));
  add(current_directory);
 
  instance_ = this;
}

GUIFileManager::~GUIFileManager ()
{
  
}

void
GUIFileManager::open_directory (const std::string& pathname)
{
  GUIDirectory* old_directory = current_directory;

  if (directories[pathname] == 0)
    {
      current_directory = directories[pathname] = new GUIDirectory(pathname);
    }
  else
    {
      current_directory = directories[pathname];
    }

  replace (old_directory, current_directory);
}

void
GUIFileManager::directory_up()
{
  std::string pathname = current_directory->get_path ();

  // FIXME: UGLY code
  if (pathname == "/")
    {
      // already at the top most directory
      return;
    }
  else
    {
      assert(*(pathname.end()-1) == '/');

      for (std::string::size_type i = pathname.size() - 2; i >= 0; --i)
        {
          if (pathname[i] == '/') // Found second '/'
            {
              pathname = pathname.substr(0, i+1);
              break;
            }
        }

      std::cout << "Directory Up: " << current_directory->get_path () << " -> " << pathname << std::endl;
      open_directory (pathname);
    }
}

void
GUIFileManager::draw_overlay (GraphicContext* gc)
{
  gc->draw_string(200, 10, current_directory->get_path());
}

/* EOF */
