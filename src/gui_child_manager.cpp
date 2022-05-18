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

#include "gui_child_manager.hpp"

#include <algorithm>

#include "graphic_context.hpp"
#include "colors.hpp"

GUIChildManager::GUIChildManager() :
  GUIComponent(),
  m_components(),
  m_current_component(nullptr)
{
}

GUIChildManager::~GUIChildManager ()
{
}

void
GUIChildManager::add(std::unique_ptr<GUIComponent> comp)
{
  m_components.push_back(std::move(comp));
}

void
GUIChildManager::remove(GUIComponent* comp)
{
  std::erase_if(m_components, [comp](auto&& item){
    return item.get() == comp;
  });

  if (m_current_component == comp) {
    m_current_component = nullptr;
  }
}

void
GUIChildManager::clear()
{
  m_components.clear();
  m_current_component = nullptr;
}

void
GUIChildManager::draw(GraphicContext& parent_gc)
{
  GCZoomState zoom(geom::frect(geom::fsize(parent_gc.get_width(), parent_gc.get_height())));
  zoom.set_offset(m_geometry.topleft());

  ZoomGraphicContext gc(parent_gc, zoom);

  parent_gc.draw_fill_rect(m_geometry, g_style.button_bg_passive);
  parent_gc.draw_rect(m_geometry, g_style.button_fg_passive);

  parent_gc.draw_fill_rect(m_geometry, g_style.button_bg_hover);
  parent_gc.draw_rect(m_geometry, g_style.button_fg_passive);

  for (auto i = m_components.rbegin(); i != m_components.rend(); ++i)
  {
    (*i)->draw(gc);
  }

  draw_overlay(parent_gc);
}

void
GUIChildManager::on_primary_button_press(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_primary_button_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_primary_button_release(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_primary_button_release(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_press(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_secondary_button_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_release(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_secondary_button_release(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_delete_press(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_delete_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_fix_press(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_fix_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::on_mouse_enter()
{
}

void
GUIChildManager::on_mouse_leave()
{
}

void
GUIChildManager::wheel_up(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->wheel_up(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::wheel_down(geom::fpoint const& pos)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->wheel_down(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
GUIChildManager::scroll_left()
{
  /*  for (auto i = m_components.begin(); i != m_components.end(); ++i)
      {
      if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
      {
      (*i)->scroll_left(pos - geom::foffset(m_geometry.topleft()));
      return;
      }
      }*/
}

void
GUIChildManager::scroll_right()
{
  /*
    for (auto i = m_components.begin(); i != m_components.end(); ++i)
    {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
    (*i)->scroll_right(pos - geom::foffset(m_geometry.topleft()));
    return;
    }
    }*/
}

void
GUIChildManager::scroll_up()
{
  /*  for (auto i = m_components.begin(); i != m_components.end(); ++i)
      {
      if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
      {
      (*i)->scroll_down(pos - geom::foffset(m_geometry.topleft()));
      return;
      }
      }*/
}

void
GUIChildManager::scroll_down()
{
  /*
    for (auto i = m_components.begin(); i != m_components.end(); ++i)
    {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
    (*i)->scroll_down(pos - geom::foffset(m_geometry.topleft()));
    return;
    }
    }*/
}

void
GUIChildManager::on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset)
{
  GUIComponent* const comp = find_component_at(pos);

  if (comp != m_current_component)
  {
    if (comp) {
      comp->on_mouse_enter();
    }
    if (m_current_component) {
      m_current_component->on_mouse_leave();
    }
    m_current_component = comp;
  }
  else if (comp)
  {
    comp->on_mouse_move(pos, offset);
  }
}

GUIComponent*
GUIChildManager::find_component_at(geom::fpoint const& pos) const
{
  for (auto& component : m_components)
  {
    if (component->is_at(pos - geom::foffset(m_geometry.topleft()))) {
      return component.get();
    }
  }
  return nullptr;
}

/* EOF */
