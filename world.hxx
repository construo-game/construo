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
#include "stick.hxx"
#include "particle.hxx"

/** This class holds all particles and springs */
class World
{
private:
  std::list<Particle*> particles;
  typedef std::list<Particle*>::iterator ParticleIter;

  std::list<Stick*> sticks;
  typedef std::list<Stick*>::iterator StickIter;

public:
  World ();
  ~World ();

  void draw (GraphicContext* gc);
  void update (float delta);

  /** @return the particles closed to the given coordinates */
  Particle* get_particle (int x, int y);
  void add_spring (Particle*, Particle*);
  /** The world will take care of the deletion */
  void add_particle (Particle*);

  /** removes the given particle and all objects which reference it */
  void remove_particle (Particle*);

  /** remove the given stick */
  void remove_stick (Stick*);

  /** removes everything from the world */
  void clear ();
private:
  World (const World&);
  World& operator= (const World&);
};

#endif

/* EOF */
