//  $Id$
//
//  Pingus - A free Lemmings clone
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

#include "math.hxx"
#include "world.hxx"
#include "controller.hxx"
#include "worldview_component.hxx"
#include "particle.hxx"
#include "gui_manager.hxx"
#include "worldview_select_tool.hxx"

extern GUIManager* gui_manager;
extern Controller* controller;

WorldViewSelectTool::WorldViewSelectTool (WorldViewComponent* c)
  : WorldViewTool (c)
{
  mode = IDLE_MODE;
}

WorldViewSelectTool::~WorldViewSelectTool ()
{
}

void
WorldViewSelectTool::draw_background (GraphicContext* gc)
{
  int x = worldview_component->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  int y = worldview_component->get_gc()->screen_to_world_y (input_context->get_mouse_y ());               

  if (mode == GETTING_SELECTION_MODE)
    {
      gc->draw_rect (Math::min(x, int(selection_start.x)),
                     Math::min(y, int(selection_start.y)),
                     Math::max(x, int(selection_start.x)),
                     Math::max(y, int(selection_start.y)),
                     Color(0xFFFFFF));
    }

  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      (*i)->draw_highlight (gc);
    }
}

void
WorldViewSelectTool::draw_foreground (GraphicContext* gc)
{
}

void
WorldViewSelectTool::activate ()
{
}

void
WorldViewSelectTool::deactivate ()
{
  selection.clear ();
}

void
WorldViewSelectTool::on_primary_button_press (int screen_x, int screen_y)
{
  int x = worldview_component->get_gc()->screen_to_world_x (screen_x);
  int y = worldview_component->get_gc()->screen_to_world_y (screen_y);
  
  World& world = *controller->get_world ();

  gui_manager->grab_mouse (worldview_component);

  mode = GETTING_SELECTION_MODE;

  Particle* new_current_particle = world.get_particle (x, y);
  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      if (new_current_particle == *i)
        mode = MOVING_SELECTION_MODE;
    }
      
  if (mode == GETTING_SELECTION_MODE)
    {
      selection.clear ();
      selection_start.x = x;
      selection_start.y = y;
    }
}

void
WorldViewSelectTool::on_primary_button_release (int x, int y)
{
  World& world = *controller->get_world ();
      
  gui_manager->ungrab_mouse (worldview_component);
  
  if (mode == GETTING_SELECTION_MODE)
    { 
      selection = world.get_particles (int(selection_start.x), int(selection_start.y),
                                       worldview_component->get_gc()->screen_to_world_x (x),
                                       worldview_component->get_gc()->screen_to_world_y (y));
      mode = IDLE_MODE;
    }
}

void
WorldViewSelectTool::on_secondary_button_press (int x, int y)
{
  mode = ROTATING_SELECTION_MODE;
}

void
WorldViewSelectTool::on_secondary_button_release (int x, int y)
{
}

void
WorldViewSelectTool::on_delete_press (int x, int y)
{
  World& world = *controller->get_world ();
  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      world.remove_particle(*i);
    }

  selection.clear ();
}

void
WorldViewSelectTool::on_fix_press (int x, int y)
{
  bool mark_all = false;
  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      if (!(*i)->get_fixed())
        {
          mark_all = true;
        }
    }

  if (mark_all)
    {
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->set_fixed (true);
        }
    }
  else
    {
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          (*i)->set_fixed (!(*i)->get_fixed());
        }
    }
}

void
WorldViewSelectTool::on_mouse_move (int x, int y, int of_x, int of_y)
{
  World& world = *controller->get_world ();

  if (mode == MOVING_SELECTION_MODE)
    {
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          // Will lead to round errors 
          (*i)->pos.x += of_x / worldview_component->get_gc ()->get_zoom();
          (*i)->pos.y += of_y / worldview_component->get_gc ()->get_zoom();

          std::vector<Spring*>& spring_mgr = world.get_spring_mgr();
          for (std::vector<Spring*>::iterator j = spring_mgr.begin (); 
               j != spring_mgr.end (); ++j)
            {
              if ((*j)->particles.first == *i
                  || (*j)->particles.second == *i)
                {
                  (*j)->recalc_length ();
                }
            }
        }
    }
}

/* EOF */
