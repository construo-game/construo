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

#ifndef HEADER_CONSTRUO_WORLD_RENDERER_HPP
#define HEADER_CONSTRUO_WORLD_RENDERER_HPP

#include "fwd.hpp"

class WorldRenderer
{
public:
  WorldRenderer(World const& world);

  void draw(ZoomGraphicContext& gc) const;
  void draw_springs(ZoomGraphicContext& gc) const;
  void draw_colliders(ZoomGraphicContext& gc) const;
  void draw_particles(ZoomGraphicContext& gc) const;

  /** Draw the background grid to which new dots align */
  void draw_grid(ZoomGraphicContext& gc, float grid_size, int grid_constant) const;

  /** Draw ground, aka the all overspanning collider at the bottem */
  void draw_ground(ZoomGraphicContext& gc) const;

  static void draw_particle(ZoomGraphicContext& gc, Particle const& particle);
  static void draw_particle_info(ZoomGraphicContext& gc, Particle const& particle);
  static void draw_particle_highlight(ZoomGraphicContext& gc, Particle const& particle);
  static void draw_particle_velocity_vector(ZoomGraphicContext& gc, Particle const& particle);

  static void draw_spring(ZoomGraphicContext& gc, Spring const& spring);
  static void draw_spring_highlight(ZoomGraphicContext& gc, Spring const& spring);

  static void draw_collider(ZoomGraphicContext& gc, Collider const& collider);
  static void draw_collider_highlight(ZoomGraphicContext& gc, Collider const& collider);
  static void draw_rect_collider(ZoomGraphicContext& gc, RectCollider const& collider);
  static void draw_rect_collider_highlight(ZoomGraphicContext& gc, RectCollider const& collider);

private:
  World& m_world;

private:
  WorldRenderer(const WorldRenderer&) = delete;
  WorldRenderer& operator=(const WorldRenderer&) = delete;
};

#endif

/* EOF */
