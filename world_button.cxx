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

#include "world.hxx"
#include "world_button.hxx"
#include "construo_error.hxx"

WorldButton::WorldButton (const std::string& arg_filename)
  : GUIFileButton (arg_filename)
{
  std::cout << "Creating WorldButton" << std::endl;

  try {
    world = new World(filename);
  } catch (ConstruoError& err) {
    std::cout << "ERROR: " << err.msg << std::endl;
    std::cout << "ERROR: WorldButton: Somthing went wrong loading " << filename << std::endl;
    world = 0;
  }
}

WorldButton::~WorldButton ()
{
}

void
WorldButton::draw (GraphicContext* parent_gc)
{
  ZoomGraphicContext gc;
  gc.set_parent_gc(parent_gc);

  if (world)
    {
      const WorldBoundingBox& box = world->calc_bounding_box();
      gc.zoom_to((int) box.x1, (int)box.y1,
                  (int)box.x2, (int)box.y2);
      world->draw (&gc);
      gc.draw_string (50, 50, filename);
    }
  else
    {
      // Draw an 'X' for broken levels
      gc.draw_line (0,0, gc.get_width (), gc.get_height (), Color (0xFF00FFFF));
      gc.draw_line (0,gc.get_height (), gc.get_width (), 0, Color (0xFF00FFFF));
    }
}

void
WorldButton::on_click ()
{
  std::cout << "WorldButton: detected click on: " << filename << std::endl;
}

/* EOF */
