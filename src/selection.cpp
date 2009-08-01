//  Construo - A wire-frame construction gamee
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <config.h>
#include <algorithm>
#include <math.h>
#include <map>
#include "rect.hpp"
#include "selection.hpp"
#include "particle.hpp"
#include "particle_factory.hpp"
#include "controller.hpp"

Selection::Selection ()
{
  world = 0;
}

Vector2d
Selection::get_center ()
{
  validate();

  Rect<float> rot_box((*selection.begin ())->pos.x,
                      (*selection.begin ())->pos.y,
                      (*selection.begin ())->pos.x,
                      (*selection.begin ())->pos.y);

  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      rot_box.x1 = Math::min(rot_box.x1, (*i)->pos.x);
      rot_box.y1 = Math::min(rot_box.y1, (*i)->pos.y);

      rot_box.x2 = Math::max(rot_box.x2, (*i)->pos.x);
      rot_box.y2 = Math::max(rot_box.y2, (*i)->pos.y);
    }
 
  return rot_box.get_center ();
}

void
Selection::scale (float factor, Vector2d center)
{
  validate();

  if (!selection.empty())
    {    
      for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->pos = center + (((*i)->pos - center) * factor);
          
          std::vector<Spring*>& springs = world->get_spring_mgr ();
          for (std::vector<Spring*>::iterator s = springs.begin(); s != springs.end(); ++s)
            {
              if ((*s)->particles.first == (*i) || ((*s)->particles.second == (*i)))
                {
                  (*s)->recalc_length();
                }
            }
        }
    }     
}

void
Selection::set_velocity (const Vector2d vel)
{
  for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
    (*i)->velocity = vel;
}

void
Selection::flip ()
{
  validate();

  if (!selection.empty())
    {
      float midpoint = 0.0f;
      for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          midpoint += (*i)->pos.x;
        }
      midpoint /= selection.size ();

      for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->pos.x = midpoint - ((*i)->pos.x - midpoint);
        }
    }
}

void
Selection::select_particles (Vector2d p1, Vector2d p2)
{
  world = Controller::instance()->get_world ();
  
  std::vector<Particle*> particles = world->get_particles (p1.x, p1.y,
                                                           p2.x, p2.y);

  selection = SelectionLst(particles.begin(),
                           particles.end());
}

void
Selection::duplicate ()
{
  validate();

  Controller::instance()->push_undo();
  
  // particle translation table
  std::map<Particle*, Particle*> p_trans_table;

  SelectionLst new_selection;

  std::cout << "Trying to duplicate the selection" << std::endl;
  for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      Particle* p = world->get_particle_mgr()->add_particle(**i);
      p->pos += Vector2d (50,50);
      new_selection.push_back(p);
      p_trans_table[*i] = p;

      //std::cout << "P: " << (*i)->get_id () << " New: " << p->get_id () << std::endl;
    }

  // FIXME: Warning, make sure that iterators stays intact while modifing the container
  std::vector<Spring*> springs = world->get_spring_mgr ();
  for (std::vector<Spring*>::iterator i = springs.begin (); i != springs.end (); ++i)
    {
      // both particles of the spring are in the current selection
      if (std::find (selection.begin (), selection.end (), (*i)->particles.first) != selection.end ()
          && 
          std::find (selection.begin (), selection.end (), (*i)->particles.second) != selection.end ())
        {
          world->add_spring (p_trans_table[(*i)->particles.first],
                             p_trans_table[(*i)->particles.second]);
        }
    }

  selection = new_selection;
}

bool
Selection::empty() const
{
  return selection.empty();
}

void
Selection::clear() 
{
  selection.clear();
  world = 0;
}

void
Selection::rotate (float rot_angle, Vector2d rotate_center)
{
  validate();

  for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      Vector2d& pos = (*i)->pos;
      
      pos.x -= rotate_center.x;
      pos.y -= rotate_center.y;
      
      float angle  = atan2(pos.y, pos.x) + rot_angle;
      float length = pos.norm ();

      pos.x = (cos (angle)*length) + rotate_center.x;
      pos.y = (sin (angle)*length) + rotate_center.y;
    }
}

void
Selection::validate()
{
  if (world != Controller::instance()->get_world ())
    {
      //std::cout << "World changed; " << world << " " << Controller::instance()->get_world () << std::endl;
      clear();
    }
}

void
Selection::join_doubles(float toleranz)
{
  // FIXME: Undo add undo, if stuff is going to change
  Controller::instance()->push_undo();
  World& world = *Controller::instance()->get_world ();
  
  for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      SelectionLst::iterator j = i;
      ++j;
      for (; j != selection.end (); ++j)
        {
          if (Vector2d::distance((*j)->pos, (*i)->pos) < toleranz)
            {
              // Join two particles
              std::cout << "joining particles: " << (*j)->pos << " " << (*i)->pos << std::endl;
              (*j)->pos      = ((*j)->pos + (*i)->pos) * 0.5f;
              (*j)->velocity = ((*j)->velocity + (*i)->velocity) * 0.5f;
             
              //selection.remove(*i);

              { // Everything that is connected to the particle 'i'
                // which should get removed, needs to get connected to
                // 'j'
                std::vector<Spring*>& springs = world.get_spring_mgr ();
                for (std::vector<Spring*>::iterator s = springs.begin(); s != springs.end(); ++s)
                  {
                    if ((*s)->particles.first == (*i))
                      (*s)->particles.first = (*j);
                        
                    if ((*s)->particles.second == (*i))
                      (*s)->particles.second = (*j);
                  }
              }

              world.remove_particle(*i);
            }
        }
    }
  clear();
}

/* EOF */
