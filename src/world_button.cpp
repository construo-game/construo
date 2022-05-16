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

#include "world_button.hpp"

#include "construo_error.hpp"
#include "controller.hpp"
#include "path.hpp"
#include "screen_manager.hpp"
#include "world.hpp"
#include "world_cache.hpp"
#include "world_renderer.hpp"

WorldButton::WorldButton (WorldCache& world_cache, const std::string& filename, Mode m) :
  GUIFileButton(filename),
  m_world_cache(world_cache),
  m_mode(m),
  m_basename(path_basename(filename))
{
}

WorldButton::~WorldButton ()
{
}

void
WorldButton::draw(GraphicContext& parent_gc)
{
  World const* world = m_world_cache.get(m_path);

  parent_gc.draw_fill_rect(m_geometry, Color(0xBB0000FF));

  GCZoomState zoom(m_geometry);

  if (world) {
    zoom.zoom_to(world->calc_bounding_box());
  }

  ZoomGraphicContext gc(parent_gc, zoom);

  gc.lock();

  if (world)
  {
    WorldRenderer renderer(*world);
    renderer.draw_colliders(gc);
    renderer.draw_springs(gc);
  }
  else
  {
    // Draw an 'X' for broken levels
    gc.draw_line(geom::fpoint(0, 0), geom::fpoint(gc.get_width(), gc.get_height()), Color (0xFF00FFFF));
    gc.draw_line(geom::fpoint(0, gc.get_height()), geom::fpoint(gc.get_width(), 0), Color (0xFF00FFFF));
  }

  gc.unlock();

  if (m_mouse_over) {
    parent_gc.draw_rect(m_geometry, Color (0xFFFFFFFF));
  } else {
    parent_gc.draw_rect(m_geometry, Color (0xFF0000FF));
  }

  parent_gc.draw_string(m_geometry.topleft() + geom::foffset(8.0f, m_geometry.height() + 14.0f), m_basename);
}

void
WorldButton::on_click ()
{
  if (m_mode == SAVE_BUTTON)
  {
    Controller::instance()->save_world(m_path);
    ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
  }
  else // LOAD BUTTON
  {
    Controller::instance()->load_world(m_path);
    ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
  }
}

/* EOF */
