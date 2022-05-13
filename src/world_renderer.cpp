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
    draw_particle(gc, *particle);
  }
}

void
WorldRenderer::draw_grid(ZoomGraphicContext& gc, float grid_size, int grid_constant) const
{
  Color color = g_style.grid_color;
  Color color2 = g_style.grid_color2;

  float start_x = Math::round_to_float(gc.zoom().screen_to_world_x(0), grid_size) - grid_size;
  float end_x   = Math::round_to_float(gc.zoom().screen_to_world_x(gc.zoom().bounding_width()), grid_size) + grid_size;

  float start_y = Math::round_to_float(gc.zoom().screen_to_world_y(0), grid_size) - grid_size;
  float end_y   = Math::round_to_float(gc.zoom().screen_to_world_y(gc.zoom().bounding_height()), grid_size) + grid_size;

  gc.push_quick_draw();
  for(float y = start_y; y < end_y; y += grid_size) {
    gc.draw_line(start_x, y,
                 end_x, y,
                 ((int(y / grid_size) % grid_constant) == 0) ? color2 : color, 1);
  }

  for(float x = start_x; x < end_x; x += grid_size) {
    gc.draw_line(x, start_y,
                 x, end_y,
                 ((int(x / grid_size) % grid_constant) == 0) ? color2 : color, 1);
  }

  gc.pop_quick_draw();
}

void
WorldRenderer::draw_ground(ZoomGraphicContext& gc) const
{
  GraphicContext& parent_gc = gc.get_parent_gc();

  if (gc.zoom().screen_to_world_y(parent_gc.get_height()) >= 599)
  {
    gc.draw_fill_rect(gc.zoom().screen_to_world_x(0),
                      599,
                      gc.zoom().screen_to_world_x(parent_gc.get_width()),
                      gc.zoom().screen_to_world_y(parent_gc.get_height()),
                      g_style.ground_color);

    // draw grid
    {
      gc.push_quick_draw();

      float const step_size = 100.0f;

      float const start_x = Math::round_to_float(gc.zoom().screen_to_world_x(gc.zoom().bounding_x1()), step_size) - step_size;
      float const end_x   = Math::round_to_float(gc.zoom().screen_to_world_x(gc.zoom().bounding_x2()), step_size) + step_size;

      float const start_y = 599;
      float const end_y   = Math::round_to_float(gc.zoom().screen_to_world_y(gc.zoom().bounding_y2()), step_size) + step_size;

      for(float y = start_y; y < end_y; y += step_size) {
        gc.draw_line(start_x, y,
                     end_x, y,
                     g_style.ground_grid_color, 1);
      }

      for(float x = start_x; x < end_x; x += step_size) {
        gc.draw_line(x, start_y,
                     x, end_y,
                     g_style.ground_grid_color, 1);
      }
      gc.pop_quick_draw();
    }

    gc.draw_rect(gc.zoom().screen_to_world_x(0),
                 599,
                 gc.zoom().screen_to_world_x(parent_gc.get_width()),
                 gc.zoom().screen_to_world_y(parent_gc.get_height()),
                 g_style.rect_collider_bg);
  }
}

void
WorldRenderer::draw_particle(ZoomGraphicContext& gc, Particle const& particle)
{
  if (particle.pos.y < 598.5f)
  {
    if (particle.fixed)
    {
      gc.get_parent_gc().draw_fill_circle (gc.zoom().world_to_screen(particle.pos),
                                           4,
                                           Color(0.6f, 0.6f, 0.6f));
    }
    else
    {
      gc.get_parent_gc().draw_fill_circle (gc.zoom().world_to_screen(particle.pos),
                                           std::max(3.0f, particle.get_mass()),
                                           Color(1.0f, 0.0f, 0.0f));
    }
  }
}

void
WorldRenderer::draw_particle_info(ZoomGraphicContext& gc, Particle const& particle)
{
  glm::vec2 const p = gc.zoom().world_to_screen(particle.pos);

  draw_particle_velocity_vector(gc, particle);

  gc.get_parent_gc().draw_string(p + glm::vec2(20.0f, 5.0f),
                                 "Particle: " + glm::to_string(particle.pos));
  gc.get_parent_gc().draw_string(p + glm::vec2(20.0f, 25.0f),
                                 "Fixed:    " + std::to_string(static_cast<int>(particle.fixed)));
  gc.get_parent_gc().draw_string(p + glm::vec2(20.0f, 45.0f),
                                 "Mass :    " + std::to_string(particle.get_mass()));
  gc.get_parent_gc().draw_string(p + glm::vec2(20.0f, 70.0f),
                                 "Links :    " + std::to_string(particle.spring_links));
}

void
WorldRenderer::draw_particle_highlight(ZoomGraphicContext& gc, Particle const& particle)
{
  gc.get_parent_gc().draw_fill_circle(gc.zoom().world_to_screen(particle.pos),
                                      std::max(6.0f, particle.get_mass() + 3),
                                      g_style.highlight);
}

void
WorldRenderer::draw_particle_velocity_vector(ZoomGraphicContext& gc, Particle const& particle)
{
  gc.draw_line(particle.pos.x, particle.pos.y,
               particle.pos.x + particle.velocity.x, particle.pos.y + particle.velocity.y,
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
    gc.GraphicContext::draw_line(spring.particles.first->pos,
                                 spring.particles.second->pos,
                                 Color(color, 1.0f - color, 0.0f),
                                 2);
  }
}

void
WorldRenderer::draw_spring_highlight(ZoomGraphicContext& gc, Spring const& spring)
{
  gc.GraphicContext::draw_line(spring.particles.first->pos,
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
  gc.draw_fill_rect(collider.x1, collider.y1, collider.x2, collider.y2, g_style.rect_collider_bg);
  gc.draw_rect(collider.x1, collider.y1, collider.x2, collider.y2, g_style.rect_collider_fg);
}

void
WorldRenderer::draw_rect_collider_highlight(ZoomGraphicContext& gc, RectCollider const& collider)
{
  //gc.draw_fill_rect (x1, y1, x2, y2, g_style.rect_collider_bg);
  gc.draw_rect(collider.x1, collider.y1, collider.x2, collider.y2, g_style.selection_rect);
}

/* EOF */
