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
  GCZoomState zoom(0, 0, parent_gc.get_width(), parent_gc.get_height());
  zoom.set_offset(m_geometry.left(), m_geometry.top());

  ZoomGraphicContext gc(parent_gc, zoom);

  parent_gc.draw_fill_rect(m_geometry.left(), m_geometry.top(),
                           m_geometry.left() + m_geometry.width(), m_geometry.top() + m_geometry.height(),
                           g_style.button_bg_passive);
  parent_gc.draw_rect(m_geometry.left(), m_geometry.top(),
                      m_geometry.left() + m_geometry.width(), m_geometry.top() + m_geometry.height(),
                      g_style.button_fg_passive);

  parent_gc.draw_fill_rect(m_geometry.left(), m_geometry.top(),
                           m_geometry.left() + m_geometry.width(), m_geometry.top(),
                           g_style.button_bg_hover);

  parent_gc.draw_rect(m_geometry.left(), m_geometry.top(),
                      m_geometry.left() + m_geometry.width(), m_geometry.top(),
                      g_style.button_fg_passive);

  for (auto i = m_components.rbegin(); i != m_components.rend(); ++i)
  {
    (*i)->draw(gc);
  }

  draw_overlay(parent_gc);
}

void
GUIChildManager::on_primary_button_press(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_primary_button_press(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::on_primary_button_release(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_primary_button_release(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_press(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_secondary_button_press(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_release(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_secondary_button_release(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::on_delete_press(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_delete_press(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::on_fix_press(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->on_fix_press(x - m_geometry.left(), y - m_geometry.top());
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
GUIChildManager::wheel_up(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->wheel_up(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::wheel_down(float x, float y)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
      (*i)->wheel_down(x - m_geometry.left(), y - m_geometry.top());
      return;
    }
  }
}

void
GUIChildManager::scroll_left()
{
  /*  for (auto i = m_components.begin(); i != m_components.end(); ++i)
      {
      if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
      {
      (*i)->scroll_left(x - m_geometry.left(), y - m_geometry.top());
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
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
    (*i)->scroll_right(x - m_geometry.left(), y - m_geometry.top());
    return;
    }
    }*/
}

void
GUIChildManager::scroll_up()
{
  /*  for (auto i = m_components.begin(); i != m_components.end(); ++i)
      {
      if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
      {
      (*i)->scroll_down(x - m_geometry.left(), y - m_geometry.top());
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
    if ((*i)->is_at(x - m_geometry.left(), y - m_geometry.top()))
    {
    (*i)->scroll_down(x - m_geometry.left(), y - m_geometry.top());
    return;
    }
    }*/
}

void
GUIChildManager::on_mouse_move(float x, float y, float of_x, float of_y)
{
  GUIComponent* const comp = find_component_at(x, y);

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
    comp->on_mouse_move(x, y, of_x, of_y);
  }
}

GUIComponent*
GUIChildManager::find_component_at(float x, float y)
{
  for (auto& component : m_components)
  {
    if (component->is_at(x - m_geometry.left(), y - m_geometry.top())) {
      return component.get();
    }
  }
  return nullptr;
}

/* EOF */
