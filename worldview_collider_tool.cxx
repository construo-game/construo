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

#include "construo.hxx"
#include "input_context.hxx"
#include "controller.hxx"
#include "colors.hxx"
#include "world.hxx"
#include "world_gui_manager.hxx"
#include "worldview_component.hxx"
#include "worldview_collider_tool.hxx"

WorldViewColliderTool::WorldViewColliderTool ()
{
  creating_rect = false;
  to_delete_collider = 0;
  move_collider      = 0;
}

WorldViewColliderTool::~WorldViewColliderTool ()
{
}

void
WorldViewColliderTool::draw_background (ZoomGraphicContext* gc)
{
  Vector2d mouse_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());
  if (creating_rect)
    {
      gc->GraphicContext::draw_rect(click_pos, mouse_pos, Colors::selection_rect);
    }
}

Collider*
WorldViewColliderTool::get_collider (const Vector2d& pos)
{
  World& world = *Controller::instance()->get_world();
  World::Colliders& colliders = world.get_colliders();
  for (World::Colliders::reverse_iterator i = colliders.rbegin ();
       i != colliders.rend(); ++i)
    {
      if ((*i)->is_at(pos))
        return *i;
    }
  return 0;
}

void
WorldViewColliderTool::draw_foreground (ZoomGraphicContext* gc)
{
  Vector2d mouse_pos
    = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ()); 
  Collider* collider = get_collider (mouse_pos);

  if (collider)
    collider->draw_highlight(gc);
}

void
WorldViewColliderTool::on_primary_button_press (int x, int y)
{
  WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());

  click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());

  if ((move_collider = get_collider (click_pos)) != 0)
    {
      // click_pos Offset, not position
      click_pos = click_pos - move_collider->get_pos();
      creating_rect = false;
    }
  else
    {
      creating_rect = true;
    }
}

void
WorldViewColliderTool::on_primary_button_release (int x, int y)
{
  WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());

  if (creating_rect)
    {
      Vector2d pos2 = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());
      World& world = *Controller::instance()->get_world();

      if (fabs(pos2.x - click_pos.x) < 15
          || fabs(pos2.y - click_pos.y) < 15)
        {
          std::cout << "Rect collider to small, not inserting" << std::endl;
        }
      else
        {
          world.add_rect_collider (click_pos, pos2);
        }
    }

  creating_rect = false;
  move_collider = 0;
}

void
WorldViewColliderTool::on_mouse_move (int x, int y, int of_x, int of_y)
{
  Vector2d current_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(x,y));

  if (move_collider)
    {
      move_collider->set_pos(current_pos - click_pos);
    }
}

void
WorldViewColliderTool::on_secondary_button_press (int x, int y)
{
  to_delete_collider = get_collider(WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(x, y)));
}

void
WorldViewColliderTool::on_secondary_button_release (int x, int y)
{
  World& world = *Controller::instance()->get_world();

  if (to_delete_collider
      == get_collider(WorldViewComponent::instance()->get_gc()->screen_to_world(Vector2d(x, y))))
    {
      world.remove_collider(to_delete_collider);
    }
  to_delete_collider = 0;
}

/* EOF */
