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
#include "zoom_state.hpp"

namespace construo {

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
WorldButton::rebuild_preview(World const* world)
{
  m_preview_lines.clear();
  m_preview_dots.clear();
  m_preview_broken = (world == nullptr);
  m_preview_geom = m_geometry;
  m_preview_valid = true;

  if (!world) {
    return;
  }

  ZoomState zoom(m_geometry);
  zoom.zoom_to(world->calc_bounding_box());

  auto to_screen = [&](glm::vec2 const& p) {
    return zoom.world_to_screen(geom::fpoint(p.x, p.y));
  };

  // Springs → one batched line list (screen space).
  auto const& springs = world->springs();
  m_preview_lines.reserve(springs.size());
  for (auto const& spring : springs) {
    if (!spring) {
      continue;
    }
    auto* a = spring->particles.first;
    auto* b = spring->particles.second;
    if (!a || !b) {
      continue;
    }
    m_preview_lines.push_back({to_screen(a->pos), to_screen(b->pos)});
  }

  // Particles as small circles (batched outline is enough for a thumb).
  auto const& particles = world->particles();
  m_preview_dots.reserve(particles.size());
  for (auto const& particle : particles) {
    if (!particle) {
      continue;
    }
    float r = particle->fixed ? 3.0f : 2.5f;
    m_preview_dots.push_back({to_screen(particle->pos), r});
  }
}

void
WorldButton::draw(GraphicContext& parent_gc)
{
  World const* world = m_world_cache.get(m_path);

  if (!m_preview_valid || m_preview_geom.left() != m_geometry.left()
      || m_preview_geom.top() != m_geometry.top()
      || m_preview_geom.width() != m_geometry.width()
      || m_preview_geom.height() != m_geometry.height()) {
    rebuild_preview(world);
  }

  parent_gc.draw_fill_rect(m_geometry, Color(0xBB0000FF));

  parent_gc.push_quick_draw();
  parent_gc.set_clip_rect(m_geometry);

  if (m_preview_broken) {
    parent_gc.draw_line(m_geometry.topleft(), m_geometry.bottomright(), Color(0xFF00FFFF));
    parent_gc.draw_line(geom::fpoint(m_geometry.left(), m_geometry.bottom()),
                        geom::fpoint(m_geometry.right(), m_geometry.top()),
                        Color(0xFF00FFFF));
  } else {
    if (!m_preview_lines.empty()) {
      parent_gc.draw_lines(m_preview_lines, Color(0.2f, 0.9f, 0.2f), 1);
    }
    if (!m_preview_dots.empty()) {
      parent_gc.draw_circles(m_preview_dots, Color(1.0f, 0.3f, 0.3f));
    }
  }

  parent_gc.clear_clip_rect();
  parent_gc.pop_quick_draw();

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

} // namespace construo

/* EOF */
