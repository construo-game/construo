// Construo - A wire-frame construction game
// Copyright (C) 2002-2022 Ingo Ruhnke <grumbel@gmail.com>
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

#include "world_renderer.hpp"

#include <assert.h>

#include <glm/gtx/io.hpp>
#include <glm/gtx/string_cast.hpp>

#include "colors.hpp"
#include "particle_factory.hpp"
#include "rect_collider.hpp"
#include "world.hpp"
#include "zoom_graphic_context.hpp"

namespace construo {

WorldRenderer::WorldRenderer(World const& world) :
  m_world(const_cast<World&>(world))
{
}

void
WorldRenderer::draw(ZoomGraphicContext& gc) const
{
  draw_colliders(gc);
  draw_springs(gc);
  draw_particles(gc);
}

void
WorldRenderer::draw_springs(ZoomGraphicContext& gc) const
{
  auto const& springs = m_world.springs();

#ifdef NEW_SPRING_CODE
  std::vector<GraphicContext::Line> lines (springs.size());

  glm::vec2 dist = springs[0]->particles.first->pos - springs[0]->particles.second->pos;
  float stretch = std::fabs(glm::length(dist)/springs[0]->length - 1.0f) * 10.0f;
  float color = std::fabs((stretch/springs[0]->max_stretch));

  for (unsigned int i = 0; i < springs.size(); ++i)
  {
    //(*i)->draw (gc);
    lines[i].x1 = springs[i]->particles.first->pos.x;
    lines[i].y1 = springs[i]->particles.first->pos.y;
    lines[i].x2 = springs[i]->particles.second->pos.x;
    lines[i].y2 = springs[i]->particles.second->pos.y;
  }
  gc.draw_lines (lines, Color(color, 1.0f - color, 0.0f), 2);
#else
  for (auto const& spring : springs)
  {
    draw_spring(gc, *spring);
  }
#endif
}

void
WorldRenderer::draw_colliders(ZoomGraphicContext& gc) const
{
  for (auto const& collider : m_world.colliders()) {
    draw_collider(gc, *collider);
  }
}

void
WorldRenderer::draw_particles(ZoomGraphicContext& gc) const
{
  for (auto const& particle : m_world.particles()) {
    if (particle->fixed)
    {
      gc.get_parent_gc().draw_fill_circle(gc.zoom().world_to_screen(particle->pos),
                                          4,
                                          Color(0.6f, 0.6f, 0.6f));
    }
    else
    {
      gc.get_parent_gc().draw_fill_circle(gc.zoom().world_to_screen(particle->pos),
                                          std::max(3.0f, gc.zoom().get_scale() * mass_to_radius(particle->get_mass())),
                                          Color(1.0f, 0.0f, 0.0f));
    }
  }
}

void
WorldRenderer::draw_grid(ZoomGraphicContext& gc, float grid_size, int grid_constant) const
{
  Color color = g_style.grid_color;
  Color color2 = g_style.grid_color2;

  geom::fpoint const start = Math::round_to(gc.zoom().screen_to_world({0.0f, 0.0f}), geom::fsize(grid_size, grid_size)) - geom::foffset(grid_size, grid_size);
  geom::fpoint const end = Math::round_to(gc.zoom().screen_to_world(gc.zoom().bounding_box().size()), geom::fsize(grid_size, grid_size)) + geom::foffset(grid_size, grid_size);

  gc.push_quick_draw();
  for(float y = start.y(); y < end.y(); y += grid_size) {
    gc.draw_line(geom::fpoint(start.x(), y),
                 geom::fpoint(end.x(), y),
                 ((int(y / grid_size) % grid_constant) == 0) ? color2 : color, 1);
  }

  for(float x = start.x(); x < end.x(); x += grid_size) {
    gc.draw_line(geom::fpoint(x, start.y()),
                 geom::fpoint(x, end.y()),
                 ((int(x / grid_size) % grid_constant) == 0) ? color2 : color, 1);
  }

  gc.pop_quick_draw();
}

void
WorldRenderer::draw_ground(ZoomGraphicContext& gc) const
{
  GraphicContext& parent_gc = gc.get_parent_gc();

  if (gc.zoom().screen_to_world(geom::fpoint(0, parent_gc.geometry().height())).y() < 599) {
    // ground is not in view, so skip it
    return;
  }

  gc.draw_fill_rect(geom::frect(geom::fpoint(gc.zoom().screen_to_world(geom::fpoint(0.0f, 0.0f)).x(), 599),
                                gc.zoom().screen_to_world(geom::fpoint(parent_gc.geometry().size()))),
                      g_style.ground_color);

    // draw grid
    {
      gc.push_quick_draw();

      geom::fsize const step_size(100.0f, 100.0f);

      geom::fpoint const start((Math::round_to(gc.zoom().screen_to_world(gc.zoom().bounding_box().topleft()), step_size) - geom::fsize(step_size)).x(),
                               599);
      geom::fpoint const end = Math::round_to(gc.zoom().screen_to_world(gc.zoom().bounding_box().bottomright()), step_size) + step_size;

      for(float y = start.y(); y < end.y(); y += step_size.height()) {
        gc.draw_line(geom::fpoint(start.x(), y),
                     geom::fpoint(end.x(), y),
                     g_style.ground_grid_color, 1);
      }

      for(float x = start.x(); x < end.x(); x += step_size.width()) {
        gc.draw_line(geom::fpoint(x, start.y()),
                     geom::fpoint(x, end.y()),
                     g_style.ground_grid_color, 1);
      }
      gc.pop_quick_draw();
    }

    gc.draw_rect(geom::frect(geom::fpoint(gc.zoom().screen_to_world(geom::fpoint(0.0f, 0.0f)).x(),
                                          599),
                             gc.zoom().screen_to_world(geom::fpoint(parent_gc.geometry().size()))),
                 g_style.rect_collider_bg);
}

void
WorldRenderer::draw_particle_info(ZoomGraphicContext& gc, Particle const& particle)
{
  geom::fpoint const p = gc.zoom().world_to_screen(particle.pos);

  draw_particle_velocity_vector(gc, particle);

  gc.get_parent_gc().draw_string(p + geom::foffset(20.0f, 5.0f),
                                 "Particle: " + glm::to_string(particle.pos));
  gc.get_parent_gc().draw_string(p + geom::foffset(20.0f, 25.0f),
                                 "Fixed:    " + std::to_string(static_cast<int>(particle.fixed)));
  gc.get_parent_gc().draw_string(p + geom::foffset(20.0f, 45.0f),
                                 "Mass :    " + std::to_string(particle.get_mass()));
  gc.get_parent_gc().draw_string(p + geom::foffset(20.0f, 70.0f),
                                 "Links :    " + std::to_string(particle.spring_links));
}

void
WorldRenderer::draw_particle_highlight(ZoomGraphicContext& gc, Particle const& particle)
{
  gc.get_parent_gc().draw_fill_circle(gc.zoom().world_to_screen(particle.pos),
                                      std::max(6.0f, gc.zoom().get_scale() * mass_to_radius(particle.get_mass()) + 3),
                                      g_style.highlight);
}

void
WorldRenderer::draw_particle_velocity_vector(ZoomGraphicContext& gc, Particle const& particle)
{
  gc.draw_line(particle.pos, particle.pos + particle.velocity,
               Color(0.0f, 0.0f, 1.0f));
}

void
WorldRenderer::draw_spring(ZoomGraphicContext& gc, Spring const& spring)
{
  glm::vec2 const dist = spring.particles.first->pos - spring.particles.second->pos;
  float const stretch = std::fabs(glm::length(dist) / spring.length - 1.0f) * 10.0f;

  float const color = std::fabs((stretch / spring.max_stretch));

  if (spring.particles.first->pos.y  < 598.5f ||
      spring.particles.second->pos.y < 598.5f)
  {
    gc.draw_line(spring.particles.first->pos,
                 spring.particles.second->pos,
                 Color(color, 1.0f - color, 0.0f),
                 2);
  }
}

void
WorldRenderer::draw_spring_highlight(ZoomGraphicContext& gc, Spring const& spring)
{
  gc.draw_line(spring.particles.first->pos,
                               spring.particles.second->pos,
                               g_style.highlight, 4);
}

void
WorldRenderer::draw_collider(ZoomGraphicContext& gc, Collider const& collider)
{
  if (RectCollider const* rect_collider = dynamic_cast<RectCollider const*>(&collider)) {
    draw_rect_collider(gc, *rect_collider);
  } else {
    assert(false && "unknown collider type");
  }
}

void
WorldRenderer::draw_collider_highlight(ZoomGraphicContext& gc, Collider const& collider)
{
  if (RectCollider const* rect_collider = dynamic_cast<RectCollider const*>(&collider)) {
    draw_rect_collider_highlight(gc, *rect_collider);
  } else {
    assert(false && "unknown collider type");
  }
}

void
WorldRenderer::draw_rect_collider(ZoomGraphicContext& gc, RectCollider const& collider)
{
  gc.draw_fill_rect(geom::frect(collider.x1, collider.y1, collider.x2, collider.y2), g_style.rect_collider_bg);
  gc.draw_rect(geom::frect(collider.x1, collider.y1, collider.x2, collider.y2), g_style.rect_collider_fg);
}

void
WorldRenderer::draw_rect_collider_highlight(ZoomGraphicContext& gc, RectCollider const& collider)
{
  //gc.draw_fill_rect (x1, y1, x2, y2, g_style.rect_collider_bg);
  gc.draw_rect(geom::frect(collider.x1, collider.y1, collider.x2, collider.y2), g_style.selection_rect);
}

float
WorldRenderer::mass_to_radius(float mass)
{
  float const scale_factor = 5.0f;
  return std::sqrt(mass) * scale_factor;
}

} // namespace construo

/* EOF */
