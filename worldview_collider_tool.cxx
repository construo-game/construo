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
}

WorldViewColliderTool::~WorldViewColliderTool ()
{
}

void
WorldViewColliderTool::draw_background (ZoomGraphicContext* gc)
{
  Vector2d pos2 = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());
  if (creating_rect)
    {
      gc->GraphicContext::draw_rect(click_pos, pos2, Colors::selection_rect);
    }
}

void
WorldViewColliderTool::draw_foreground (ZoomGraphicContext* gc)
{
}

void
WorldViewColliderTool::on_primary_button_press (int x, int y)
{
  WorldGUIManager::instance()->grab_mouse (WorldViewComponent::instance());
  
  click_pos = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());
  creating_rect = true;
}

void
WorldViewColliderTool::on_primary_button_release (int x, int y)
{
  WorldGUIManager::instance()->ungrab_mouse (WorldViewComponent::instance());

  Vector2d pos2 = WorldViewComponent::instance()->get_gc()->screen_to_world(input_context->get_mouse_pos ());
  World& world = *Controller::instance()->get_world();
  world.add_rect_collider (click_pos, pos2);
  creating_rect = false;
}

/* EOF */
