//  $Id$
// 
//  Construo - A wire-frame construction game
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

#ifndef HEADER_CONSTRUO_WORLD_HXX
#define HEADER_CONSTRUO_WORLD_HXX

#include <list>
#include "spring.hxx"
#include "particle.hxx"

class Particle;
class ParticleFactory;
class Spring;

/** This class holds all particles and springs */
class World
{
private:
  bool has_been_run;
  ParticleFactory* particle_mgr;

  std::list<Spring*> springs;
  typedef std::list<Spring*>::iterator SpringIter;
  typedef std::list<Spring*>::const_iterator CSpringIter;

  void parse_scene (lisp_object_t* lst);
  void parse_springs (lisp_object_t* lst);
  void parse_particles (lisp_object_t* lst);

public:
  World ();
  World (const World& w);
  World (const std::string& filename);
  ~World ();

  void draw (GraphicContext* gc);
  void update (float delta);

  World* duplicate () { return new World (*this); }

  /** @return the particles closed to the given coordinates */
  Particle* get_particle (int x, int y);
  void add_spring (Particle*, Particle*);

  /** removes the given particle and all objects which reference it */
  void remove_particle (Particle*);

  /** remove the given spring */
  void remove_spring (Spring*);

  ParticleFactory* get_particle_mgr() { return particle_mgr; }

  /** removes everything from the world */
  void clear ();
  
  bool get_has_been_run () { return has_been_run; }

  /** Sets the velocity of all particles to zero, usefull if the
      particles are getting out of order (aka. explode). Also usefull
      to fix broken model files */
  void zero_out_velocity ();

  void write_lisp (const std::string& filename);
private:
  World& operator= (const World&);
};

#endif

/* EOF */
