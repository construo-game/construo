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

#include "gui_file_manager.hxx"
#include "gui_directory_button.hxx"

GUIDirectoryButton::GUIDirectoryButton (const std::string& arg_filename)
  : GUIFileButton (arg_filename)
{
}

GUIDirectoryButton::~GUIDirectoryButton ()
{
}

void
GUIDirectoryButton::draw (GraphicContext* parent_gc)
{
  //std::cout << "GUIDirectoryButton: " << filename << std::endl;
  parent_gc->draw_fill_rect (x_pos, y_pos, 
                             x_pos +  width, y_pos + height,
                             Color (0xBB0000FF));

  parent_gc->draw_string (x_pos + 40, y_pos + 20, "..:: Directory ::..");
  parent_gc->draw_string (x_pos + 30, y_pos + 40, filename);

  if (mouse_over)
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFFFFFFFF));
  else
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFF0000FF));
}

void
GUIDirectoryButton::on_click()
{
  std::cout << "Click on GUIDirectoryButton detected" << std::endl;
  GUIFileManager::instance()->open_directory(filename);
}

/* EOF */
