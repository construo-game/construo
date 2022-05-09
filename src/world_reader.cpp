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

#include "world_reader.hpp"

#include <stdexcept>

#include <logmich/log.hpp>

#include "construo.hpp"
#include "construo_error.hpp"
#include "reader.hpp"
#include "system_context.hpp"
#include "world.hpp"

WorldReader::WorldReader() :
  m_world(),
  m_version(0)
{
}

std::unique_ptr<World>
WorldReader::from_file(std::string const& filename)
{
  log_info("World: Loading '{}'...", filename);

  m_world = std::make_unique<World>();
  ReaderDocument doc = ReaderDocument::from_file(g_system_context->translate_filename(filename));
  if (doc.get_name() != "construo-scene") {
    throw std::runtime_error("World: Read error in " + filename + ". Couldn't find 'construo-scene'");
  }

  parse_scene(doc.get_mapping());

  //ConstruoAssert(m_particle_mgr, "No Particles given in file, load failed");

  //std::cout << "particles: " << particle_mgr->size () << std::endl;
  //std::cout << "springs:   " << springs.size () << std::endl;

  return std::move(m_world);
}

void
WorldReader::parse_scene(ReaderMapping const& reader)
{
  reader.read("version", m_version);

  if (ReaderCollection particles = reader.get<ReaderCollection>("particles")) {
    parse_particles(particles);
  }

  if (ReaderCollection springs = reader.get<ReaderCollection>("springs")) {
    parse_springs(springs);
  }

  if (ReaderCollection colliders = reader.get<ReaderCollection>("colliders")) {
    parse_colliders(colliders);
  }
}

void
WorldReader::parse_spring(ReaderMapping const& reader)
{
  int first_id = -1;
  int second_id = -1;
  float length = -1;
  float stiffness = 50.0f;
  float max_stretch = 0.15f;
  float damping = 0.1f;

  reader.read("first", first_id);
  reader.read("second", second_id);
  reader.read("length", length);
  reader.read("stiffness", stiffness);
  reader.read("damping", damping);
  reader.read("maxstretch", max_stretch);

  m_world->add_spring(first_id, second_id, length, stiffness, damping, max_stretch);
}

void
WorldReader::parse_springs(ReaderCollection const& collection)
{
  for (ReaderObject const& item : collection.get_objects()) {
    parse_spring(item.get_mapping());
  }
}

void
WorldReader::parse_colliders(ReaderCollection const& collection)
{
  for(auto const& item : collection.get_objects())
  {
    ReaderMapping const& reader = item.get_mapping();

    if (item.get_name() == "rect")
    {
      glm::vec2 pos1(0.0f, 0.0f);
      glm::vec2 pos2(0.0f, 0.0f);
      if (reader.read("pos1", pos1) == false ||
          reader.read("pos2", pos2) == false) {
        throw ConstruoError("RectCollider entry incomplete");
      }
      m_world->add_rect_collider(pos1, pos2);
    }
    else
    {
      std::cout << "WARNING: Unknown collider type '" << item.get_name()
                << "' skipping" << std::endl;
    }
  }
}

void
WorldReader::parse_particles(ReaderCollection const& collection)
{
  for(ReaderObject const& item : collection.get_objects()) {
    ReaderMapping const& reader = item.get_mapping();

    glm::vec2 pos(0.0f, 0.0f);
    glm::vec2 velocity(0.0f, 0.0f);
    float mass = 1.0f/10.0f;
    bool fixed = false;
    int id = -1;

    reader.read("pos", pos);
    reader.read("velocity", velocity);
    reader.read("mass", mass);
    reader.read("fixed", fixed);
    reader.read("id", id);

    switch (m_version) {
      case 0:
      case 1:
      case 2:
        mass = 1.0f/10.0f;
        break;
    }

    m_world->add_particle(id, pos, velocity, mass, fixed);
  }
}

/* EOF */
