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

#ifndef HEADER_CONSTRUO_WORLDVIEW_INSERT_TOOL_HPP
#define HEADER_CONSTRUO_WORLDVIEW_INSERT_TOOL_HPP

#include "worldview_tool.hpp"

class Particle;

/** */
class WorldViewInsertTool : public WorldViewTool
{
private:
  /** The last particle that got inserted or marked by a click, it is
      used as the first particle of the newly created spring */
  Particle* current_particle;
  
  /** mass of the particles that are inserted on a primary press */
  float particle_mass;

public:
  WorldViewInsertTool ();
  ~WorldViewInsertTool ();

  void draw_background (ZoomGraphicContext* gc);
  void draw_foreground (ZoomGraphicContext* gc);

  void on_primary_button_press (int x, int y);
  void on_primary_button_release (int x, int y);

  void on_secondary_button_press (int x, int y);
  void on_secondary_button_release (int x, int y);

  void on_delete_press (int x, int y);
  void on_fix_press (int x, int y);

  void  set_particle_mass (float p) { particle_mass = p; }
  float get_particle_mass () { return particle_mass; }
};

#endif

/* EOF */
