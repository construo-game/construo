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

#include "world_writer.hpp"

#include <logmich/log.hpp>

#include "construo.hpp"
#include "particle_factory.hpp"
#include "rect_collider.hpp"
#include "system_context.hpp"
#include "world.hpp"
#include "writer.hpp"

namespace construo {

WorldWriter::WorldWriter(World const& world) :
  m_world(const_cast<World&>(world))
{
}

void
WorldWriter::write(std::string const& filename)
{
  log_info("World: Writing to: {}", filename);

  prio::Writer writer = prio::Writer::from_file(g_system_context->translate_filename(filename));

  writer.write_comment("Written by " PACKAGE_STRING);
  writer.begin_object("construo-scene");
  writer.write("version",  3);
  writer.write("author", std::vector<std::string>({
        g_system_context->get_user_realname(),
        g_system_context->get_user_email()}));

  writer.begin_collection("particles");
  for (auto const& particle : m_world.particles())
  {
    writer.begin_object("particle")
      .write("id", particle->get_id())
      .write("pos", particle->pos)
      .write("velocity", particle->velocity)
      .write("fixed", particle->fixed)
      .write("mass", particle->mass)
      .end_object();
  }
  writer.end_collection();

  writer.begin_collection("springs");
  for (auto const& spring : m_world.springs())
  {
    writer.begin_object("spring")
      .write("first", spring->particles.first->get_id())
      .write("second", spring->particles.second->get_id())
      .write ("length", spring->length)
      .write("stiffness", spring->stiffness)
      .write("damping", spring->damping)
      .write("maxstretch", spring->max_stretch)
      .end_object();
  }
  writer.end_collection();

  writer.begin_collection("colliders");
  for (auto const& collider : m_world.colliders())
  {
    if (RectCollider const* rect = dynamic_cast<RectCollider const*>(collider.get())) {
      writer.begin_object("rect")
        .write("pos1", glm::vec2(rect->x1, rect->y1))
        .write("pos2", glm::vec2(rect->x2, rect->y2))
        .end_object();
    } else {
      assert(false && "unknown Collider type");
    }
  }
  writer.end_collection();

  writer.end_object();
}

} // namespace construo

/* EOF */
