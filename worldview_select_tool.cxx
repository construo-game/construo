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

#include <math.h>
#include "math.hxx"
#include "world.hxx"
#include "colors.hxx"
#include "controller.hxx"
#include "worldview_component.hxx"
#include "particle.hxx"
#include "world_gui_manager.hxx"
#include "rect.hxx"
#include "worldview_select_tool.hxx"

WorldViewSelectTool::WorldViewSelectTool ()
{
  mode = IDLE_MODE;
}

WorldViewSelectTool::~WorldViewSelectTool ()
{
}

void
WorldViewSelectTool::draw_background (ZoomGraphicContext* gc)
{
  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      (*i)->draw_velocity_vector (gc);
      (*i)->draw_highlight (gc);
    }
}

void
WorldViewSelectTool::draw_foreground (ZoomGraphicContext* gc)
{
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (input_context->get_mouse_x ());
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (input_context->get_mouse_y ());               

  if (mode == GETTING_SELECTION_MODE)
    {
      gc->draw_rect (Math::min(x, click_pos.x),
                     Math::min(y, click_pos.y),
                     Math::max(x, click_pos.x),
                     Math::max(y, click_pos.y),
                     Colors::selection_rect);
    }

  if (!selection.empty())
    {
      Particle& p = **selection.begin();
      Rect<float> selection_box (p.pos.x, p.pos.y, p.pos.x, p.pos.y);
      
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          selection_box.x1 = Math::min(selection_box.x1, (*i)->pos.x);
          selection_box.y1 = Math::min(selection_box.y1, (*i)->pos.y);

          selection_box.x2 = Math::max(selection_box.x2, (*i)->pos.x);
          selection_box.y2 = Math::max(selection_box.y2, (*i)->pos.y);
        }

      float border = 20.0f / gc->get_zoom();
      gc->draw_rect (selection_box.x1 - border, selection_box.y1 - border, 
                     selection_box.x2 + border, selection_box.y2 + border, 
                     Colors::new_spring);

      if (0) // draw selection rect
        {
          float rsize = 5.0f / gc->get_zoom();
          gc->draw_fill_rect (selection_box.x1 - border - rsize, selection_box.y1 - border - rsize, 
                              selection_box.x1 - border + rsize, selection_box.y1 - border + rsize, 
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x2 + border - rsize, selection_box.y1 - border - rsize, 
                              selection_box.x2 + border + rsize, selection_box.y1 - border + rsize, 
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x1 - border - rsize, selection_box.y2 + border - rsize, 
                              selection_box.x1 - border + rsize, selection_box.y2 + border + rsize, 
                              Colors::selection_resizer);
          gc->draw_fill_rect (selection_box.x2 + border - rsize, selection_box.y2 + border - rsize, 
                              selection_box.x2 + border + rsize, selection_box.y2 + border + rsize, 
                              Colors::selection_resizer);
        }

      gc->get_parent_gc()->draw_circle(gc->world_to_screen(selection.get_center ()),
                                       8.0f, Colors::selection_rect);
      gc->get_parent_gc()->draw_circle(gc->world_to_screen(selection.get_center ()),
                                       16.0f, Colors::selection_rect);
    }
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
  float x = WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x);
  float y = WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y);
  
  World& world = *Controller::instance()->get_world ();

  WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());

  mode = GETTING_SELECTION_MODE;

  // If the mouse clicks on a particle from the selection, we move the selection
  Particle* new_current_particle = world.get_particle (x, y);
  for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
    {
      if (new_current_particle == *i)
        {
          Controller::instance()->push_undo();
          mode = MOVING_SELECTION_MODE;
          break;
        }
    }
      
  // If mouse clicks into empty space, we make a new selection 
  if (mode == GETTING_SELECTION_MODE)
    {
      selection.clear ();
      click_pos.x = x;
      click_pos.y = y;
    }
}

void
WorldViewSelectTool::on_primary_button_release (int x, int y)
{
  WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
  
  if (mode == GETTING_SELECTION_MODE)
    { 
      selection.select_particles(click_pos,
                                 WorldViewComponent::instance()->get_gc()->screen_to_world (Vector2d(x,y)));
      mode = IDLE_MODE;
    }
  else if (mode == MOVING_SELECTION_MODE)
    {
      mode = IDLE_MODE;
    }
}

void
WorldViewSelectTool::on_secondary_button_press (int screen_x, int screen_y)
{
  Controller::instance()->push_undo();

  mode = ROTATING_SELECTION_MODE;
  WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());  

  click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(screen_x, screen_y));

  if (!selection.empty())
    {
      rotate_center = selection.get_center();
    }
  else
    {
    }
}

void
WorldViewSelectTool::on_secondary_button_release (int x, int y)
{
  WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());
  mode = IDLE_MODE;
}

void
WorldViewSelectTool::on_delete_press (int x, int y)
{
  Controller::instance()->push_undo();

  World& world = *Controller::instance()->get_world ();
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
WorldViewSelectTool::on_mouse_move (int screen_x, int screen_y, int of_x, int of_y)
{
  World& world = *Controller::instance()->get_world ();

  switch (mode)
    {
    case MOVING_SELECTION_MODE:
      for (Selection::iterator i = selection.begin (); i != selection.end (); ++i)
        {
          if (WorldViewComponent::instance()->uses_grid())
            {
              (*i)->pos.x += Math::round_to(of_x / WorldViewComponent::instance()->get_gc()->get_zoom(), 10);
              (*i)->pos.y += Math::round_to(of_y / WorldViewComponent::instance()->get_gc()->get_zoom(), 10);
            }
          else
            {
              // Will lead to round errors 
              (*i)->pos.x += of_x / WorldViewComponent::instance()->get_gc ()->get_zoom();
              (*i)->pos.y += of_y / WorldViewComponent::instance()->get_gc ()->get_zoom();
            }

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
      break;
    case ROTATING_SELECTION_MODE:
      {
        Vector2d new_pos(WorldViewComponent::instance()->get_gc()->screen_to_world_x (screen_x),
                         WorldViewComponent::instance()->get_gc()->screen_to_world_y (screen_y));

        float new_angle = atan2(new_pos.y - rotate_center.y,
                                new_pos.x - rotate_center.x);
        float old_angle = atan2(click_pos.y - rotate_center.y,
                                click_pos.x - rotate_center.x);
        float rot_angle = new_angle - old_angle;

        selection.rotate (rot_angle, rotate_center);

        click_pos = new_pos;
      }
      break;
    default:
      break;
    }
}

void
WorldViewSelectTool::on_duplicate_press (int x, int y)
{
  selection.duplicate ();
}

void
WorldViewSelectTool::on_button_press (int button_id, int x, int y)
{
  Vector2d pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(x, y));

  switch (button_id)
    {
    case BUTTON_SETVELOCITY:
      selection.set_velocity (pos - selection.get_center ());
      break;
    case BUTTON_FLIP:
      selection.flip();
      break;
      break;
    default:
      break;
    }
}

void
WorldViewSelectTool::on_join_press (int x, int y)
{
  std::cout << "Join pressed" << std::endl;
  selection.join_doubles(5.0f);
}

/* EOF */
