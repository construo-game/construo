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

#include "world.hxx"
#include "controller.hxx"
#include "screen_manager.hxx"
#include "world_button.hxx"
#include "construo_error.hxx"

WorldButton::WorldButton (const std::string& arg_filename, Mode m)
  : GUIFileButton (arg_filename),
    world(0),
    file_broken(false),
    mode (m)
{
}

WorldButton::~WorldButton ()
{
  delete world;
}

void
WorldButton::load_world ()
{
  if ((world == 0 
       && !file_broken)
      || mtime != system_context->get_mtime(filename))
    {
      try {
        delete world;
        world = new World(filename);
        mtime = system_context->get_mtime(filename);
      } catch (ConstruoError& err) {
        std::cout << "ERROR: " << err.msg << std::endl;
        std::cout << "ERROR: WorldButton: Somthing went wrong loading " << filename << std::endl;
        world = 0;
        file_broken = true;
      }
    }
}

void
WorldButton::draw (GraphicContext* parent_gc)
{
  load_world();

  parent_gc->draw_fill_rect (x_pos, y_pos, 
                             x_pos +  width, y_pos + height,
                             Color (0xBB0000FF));

  ZoomGraphicContext gc (x_pos, y_pos, x_pos + width, y_pos + height);
  gc.set_parent_gc(parent_gc);

  gc.lock();

  if (world)
    {
      // FIXME: bounding box should be calculated in construtor
      const BoundingBox& box = world->calc_bounding_box();
      gc.zoom_to((int) box.x1, (int)box.y1,
                 (int)box.x2,  (int)box.y2);
      world->draw_colliders (&gc);
      world->draw_springs (&gc);
    }
  else
    {
      // Draw an 'X' for broken levels
      gc.draw_line (0,0, gc.get_width (), gc.get_height (), Color (0xFF00FFFF));
      gc.draw_line (0,gc.get_height (), gc.get_width (), 0, Color (0xFF00FFFF));
    }

  gc.unlock();

  if (mouse_over)
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFFFFFFFF));
  else
    parent_gc->draw_rect (x_pos, y_pos, 
                          x_pos +  width, y_pos + height,
                          Color (0xFF0000FF));

  parent_gc->draw_string (x_pos + 20, y_pos + 160, filename);
}

void
WorldButton::on_click ()
{
  std::cout << "WorldButton: detected click on: " << filename << std::endl;
  if (mode == SAVE_BUTTON)
    {
      Controller::instance()->save_world(filename);
      ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
    }
  else // LOAD BUTTON
    {
      Controller::instance()->load_world(filename);
      ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
    }
}

/* EOF */
