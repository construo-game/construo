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

#include <time.h>
#include "controller.hxx"
#include "screen_manager.hxx"
#include "gui_new_file_button.hxx"

GUINewFileButton::GUINewFileButton(const std::string& p)
  : GUIFileButton (p), pathname (p)
{
  
}

void
GUINewFileButton::draw(GraphicContext* parent_gc)
{
  parent_gc->draw_fill_rect (x_pos, y_pos, 
                             x_pos +  width, y_pos + height,
                             Color (0x999900FF));

  parent_gc->draw_string (x_pos + 20, y_pos + 75, "..:: Save to new file ::..");
  //parent_gc->draw_string (x_pos + 30, y_pos + 40, filename);

  if (mouse_over)
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFFFFFFFF));
  else
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFF0000FF));
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

  return pathname + "/" + std::string(buffer) + ".construo";
}

void
GUINewFileButton::on_click()
{
  std::string filename = generate_filename();
  std::cout << "Saving to: " <<  filename << std::endl;
  Controller::instance()->save_world (filename); 
  ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
}

/* EOF */
