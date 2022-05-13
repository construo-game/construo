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

#include "selection.hpp"

#include <algorithm>
#include <cmath>
#include <map>

#include <geom/rect.hpp>
#include <glm/gtx/io.hpp>
#include <logmich/log.hpp>

#include "controller.hpp"
#include "particle.hpp"
#include "particle_factory.hpp"

Selection::Selection() :
  m_selection(),
  m_world(nullptr)
{
}

glm::vec2
Selection::get_center()
{
  validate();
  if (empty()) { return glm::vec2(0.0f, 0.0f); }

  geom::frect rot_box((*m_selection.begin())->pos,
                      (*m_selection.begin())->pos);

  for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
  {
    rot_box = geom::frect(std::min(rot_box.left(), (*i)->pos.x),
                          std::min(rot_box.top(), (*i)->pos.y),
                          std::max(rot_box.right(), (*i)->pos.x),
                          std::max(rot_box.bottom(), (*i)->pos.y));
  }

  return geom::center(rot_box).as_vec();
}

void
Selection::scale(float factor, glm::vec2 center)
{
  validate();
  if (m_selection.empty()) { return; }

  for (auto i = m_selection.begin(); i != m_selection.end(); ++i)
  {
    (*i)->pos = center + (((*i)->pos - center) * factor);

    for (auto& spring : m_world->get_spring_mgr())
    {
      if (spring->particles.first == (*i) || (spring->particles.second == (*i)))
      {
        spring->recalc_length();
      }
    }
  }
}

void
Selection::set_velocity(const glm::vec2 vel)
{
  validate();
  if (m_selection.empty()) { return; }

  for (auto i = m_selection.begin (); i != m_selection.end (); ++i) {
    (*i)->velocity = vel;
  }
}

void
Selection::flip ()
{
  validate();
  if (!m_selection.empty()) { return; }

  float midpoint = 0.0f;
  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    midpoint += (*i)->pos.x;
  }
  midpoint /= static_cast<float>(m_selection.size());

  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    (*i)->pos.x = midpoint - ((*i)->pos.x - midpoint);
  }
}

void
Selection::select_particles(glm::vec2 p1, glm::vec2 p2)
{
  m_world = &Controller::instance()->get_world();

  std::vector<Particle*> particles = m_world->get_particles (p1.x, p1.y,
                                                             p2.x, p2.y);

  m_selection = SelectionLst(particles.begin(), particles.end());
}

void
Selection::duplicate ()
{
  validate();
  if (m_world == nullptr) { return; }

  Controller::instance()->push_undo();

  // particle translation table
  std::map<Particle*, Particle*> p_trans_table;

  SelectionLst new_selection;

  log_debug("Trying to duplicate the selection");
  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    Particle* p = m_world->get_particle_mgr().add_particle(**i);
    p->pos += glm::vec2 (50,50);
    new_selection.push_back(p);
    p_trans_table[*i] = p;
  }

  // FIXME: Warning, make sure that iterators stays intact while modifing the container
  for (auto& spring : m_world->get_spring_mgr())
  {
    // both particles of the spring are in the current selection
    if (std::find (m_selection.begin(), m_selection.end(), spring->particles.first) != m_selection.end ()
        &&
        std::find (m_selection.begin(), m_selection.end(), spring->particles.second) != m_selection.end ())
    {
      m_world->add_spring (p_trans_table[spring->particles.first],
                           p_trans_table[spring->particles.second]);
    }
  }

  m_selection = new_selection;
}

bool
Selection::empty() const
{
  return m_selection.empty();
}

void
Selection::clear()
{
  m_selection.clear();
  m_world = nullptr;
}

void
Selection::rotate (float rot_angle, glm::vec2 rotate_center)
{
  validate();

  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    glm::vec2& pos = (*i)->pos;

    pos.x -= rotate_center.x;
    pos.y -= rotate_center.y;

    float angle  = std::atan2(pos.y, pos.x) + rot_angle;
    float length = glm::length(pos);

    pos.x = (std::cos(angle)*length) + rotate_center.x;
    pos.y = (std::sin(angle)*length) + rotate_center.y;
  }
}

void
Selection::validate()
{
  if (m_world != &Controller::instance()->get_world())
  {
    clear();
  }
}

void
Selection::join_doubles(float toleranz)
{
  // FIXME: Undo add undo, if stuff is going to change
  Controller::instance()->push_undo();
  World& world = Controller::instance()->get_world ();

  for (auto i = m_selection.begin (); i != m_selection.end (); ++i)
  {
    for (auto j = std::next(i); j != m_selection.end (); ++j)
    {
      if (glm::distance((*j)->pos, (*i)->pos) < toleranz)
      {
        // Join two particles
        log_debug("joining particles: {} {}", (*j)->pos, (*i)->pos);

        (*j)->pos      = ((*j)->pos + (*i)->pos) * 0.5f;
        (*j)->velocity = ((*j)->velocity + (*i)->velocity) * 0.5f;

        //selection.remove(*i);

        { // Everything that is connected to the particle 'i'
          // which should get removed, needs to get connected to
          // 'j'
          for (auto& spring : world.get_spring_mgr())
          {
            if (spring->particles.first == (*i))
              spring->particles.first = (*j);

            if (spring->particles.second == (*i))
              spring->particles.second = (*j);
          }
        }

        world.remove_particle(*i);
      }
    }
  }
  clear();
}

/* EOF */
