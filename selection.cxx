//  $Id$
//
//  Construo - A wire-frame construction gamee
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

#include <map>
#include "rect.hxx"
#include "selection.hxx"
#include "particle.hxx"
#include "particle_factory.hxx"
#include "controller.hxx"

Selection::Selection ()
{
  world = 0;
}

Vector2d
Selection::get_center ()
{
  Rect<float> rot_box ((*selection.begin ())->pos.x,
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
Selection::scale (float factor)
{
  if (!selection.empty())
    {
      Particle& p = **selection.begin();
      Rect<float> selection_box (p.pos.x, p.pos.y, p.pos.x, p.pos.y);
      
      for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          selection_box.x1 = Math::min(selection_box.x1, (*i)->pos.x);
          selection_box.y1 = Math::min(selection_box.y1, (*i)->pos.y);

          selection_box.x2 = Math::max(selection_box.x2, (*i)->pos.x);
          selection_box.y2 = Math::max(selection_box.y2, (*i)->pos.y);
        }
      Vector2d midpoint = selection_box.get_center ();
      
      for (SelectionLst::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->pos = (midpoint - (*i)->pos) * factor;
        }
    }
}

void
Selection::flip ()
{
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
  
  selection = world->get_particles (p1.x, p1.y,
                                    p2.x, p2.y);
}

void
Selection::duplicate ()
{
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

      std::cout << "P: " << (*i)->get_id () << " New: " << p->get_id () << std::endl;
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

/* EOF */
