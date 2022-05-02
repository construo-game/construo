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

#ifndef HEADER_CONSTRUO_WORLD_BUTTON_HPP
#define HEADER_CONSTRUO_WORLD_BUTTON_HPP

#include "gui_file_button.hpp"

class World;

/** button in the load/save dialog */
class WorldButton : public GUIFileButton
{
public:
  enum Mode { LOAD_BUTTON, SAVE_BUTTON};

public:
  WorldButton (const std::string& arg_filename, Mode m);
  ~WorldButton ();

  void draw (GraphicContext& gc) override;
  void on_click() override;

  /** Load the given world, if not called it will be loaded on demand */
  void load_world ();

private:
  World* m_world;
  bool m_file_broken;
  unsigned long m_mtime;
  Mode m_mode;

private:
  WorldButton (const WorldButton&);
  WorldButton& operator= (const WorldButton&);
};

#endif

/* EOF */
