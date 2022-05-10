// Construo - A wire-frame construction game
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

#ifndef HEADER_CONSTRUO_WORLD_HPP
#define HEADER_CONSTRUO_WORLD_HPP

#include <memory>
#include <vector>

#include "spring.hpp"
#include "particle.hpp"
#include "collider.hpp"
#include "bounding_box.hpp"

/** This class holds all particles and springs */
class World
{
  friend class ParticleFactory;

public:
  typedef std::vector<Collider*> Colliders;
  typedef std::vector<Spring*>::iterator SpringIter;
  typedef std::vector<Spring*>::const_iterator CSpringIter;

public:
  World();
  World (const World& w);
  ~World ();

  void update (float delta);

  std::unique_ptr<World> duplicate() { return std::make_unique<World>(*this); }

  /** @return the particles closed to the given coordinates */
  Particle* get_particle(float x, float y, float capture_distance = 20.0f) const;
  /** All particles inside the given rectangle */
  std::vector<Particle*> get_particles(float x1, float y1, float x2, float y2) const;
  Spring* get_spring(float x, float y, float capture_threshold = 20.0f) const;
  Collider* get_collider(glm::vec2 const& pos) const;

  void add_rect_collider(const glm::vec2&, const glm::vec2&);
  void add_spring(Particle*, Particle*);
  void add_spring(int lhs, int rhs, float length, float stiffness, float damping, float max_stretch);
  void add_particle(int id, glm::vec2 const& pos, glm::vec2 const& velocity, float mass, bool fixed);

  /** removes the given particle and all objects/springs which
      reference to it */
  void remove_particle (Particle*);

  /** remove the given spring */
  void remove_spring (Spring*);

  /** Remove the gives collider from the world */
  void remove_collider (Collider*);

  ParticleFactory& get_particle_mgr() { return *m_particle_mgr; }
  std::vector<Spring*>& get_spring_mgr () { return m_springs; }
  Colliders& get_colliders() { return m_colliders; }

  /** removes everything from the world */
  void clear ();

  bool get_has_been_run () { return m_has_been_run; }

  /** Sets the velocity of all particles to zero, usefull if the
      particles are getting out of order (aka. explode). Also usefull
      to fix broken model files */
  void zero_out_velocity ();

  /** @return the number of particles in the world */
  int get_num_particles();

  /** @return the number of springs in the world */
  int get_num_springs();

  /** Callculate the bounding box of the world from the particle and
   *  collider it contains. */
  BoundingBox calc_bounding_box() const;

private:
  bool m_has_been_run;
  std::unique_ptr<ParticleFactory> m_particle_mgr;
  std::vector<Spring*> m_springs;
  Colliders m_colliders;

private:
  World& operator= (const World&);
};

#endif

/* EOF */
