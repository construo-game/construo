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

#include <algorithm>
#include "graphic_context.hpp"
#include "colors.hpp"
#include "gui_child_manager.hpp"

GUIChildManager::GUIChildManager (int x, int y, int width, int height) :
  GUIComponent (x, y, width, height),
  m_gc(),
  m_components(),
  m_current_component(nullptr)
{
}

GUIChildManager::~GUIChildManager ()
{
  for (auto i = m_components.begin(); i != m_components.end (); ++i)
  {
    delete *i;
  }
}

void
GUIChildManager::add (GUIComponent* comp)
{
  m_components.push_back(comp);
}

void
GUIChildManager::remove (GUIComponent* comp)
{
  // FIXME: Memory leak
  m_components.erase(std::remove(m_components.begin(), m_components.end(), comp), m_components.end());
}

void
GUIChildManager::replace(GUIComponent* old_comp, GUIComponent* new_comp)
{
  for (auto i = m_components.begin(); i != m_components.end(); ++i)
  {
    if (*i == old_comp)
    {
      *i = new_comp;
      return;
    }
  }
}

void
GUIChildManager::draw (GraphicContext* parent_gc)
{
  m_gc.set_parent_gc (parent_gc);
  m_gc.set_offset (m_x, m_y);

  parent_gc->draw_fill_rect (m_x, m_y,
                             m_x + m_width, m_y + m_height,
                             Colors::button_bg_passive);
  parent_gc->draw_rect (m_x, m_y,
                        m_x + m_width, m_y + m_height,
                        Colors::button_fg_passive);

  parent_gc->draw_fill_rect (m_x, m_y,
                             m_x + m_width, m_y,
                             Colors::button_bg_hover);

  parent_gc->draw_rect (m_x, m_y,
                        m_x + m_width, m_y,
                        Colors::button_fg_passive);

  for (auto i = m_components.rbegin (); i != m_components.rend (); ++i)
  {
    (*i)->draw(&m_gc);
  }

  draw_overlay(parent_gc);
}

void
GUIChildManager::on_primary_button_press (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_primary_button_press (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_primary_button_release (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_primary_button_release (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_press (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_secondary_button_press (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_secondary_button_release (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_secondary_button_release (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_delete_press (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_delete_press (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_fix_press (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->on_fix_press (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::on_mouse_enter ()
{
}

void
GUIChildManager::on_mouse_leave ()
{
}

void
GUIChildManager::wheel_up (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->wheel_up (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::wheel_down (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      (*i)->wheel_down (x - m_x, y - m_y);
      return;
    }
  }
}

void
GUIChildManager::scroll_left ()
{
  /*  for (auto i = m_components.begin (); i != m_components.end (); ++i)
      {
      if ((*i)->is_at (x - m_x, y - m_y))
      {
      (*i)->scroll_left (x - m_x, y - m_y);
      return;
      }
      }*/
}

void
GUIChildManager::scroll_right ()
{
  /*
    for (auto i = m_components.begin (); i != m_components.end (); ++i)
    {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
    (*i)->scroll_right (x - m_x, y - m_y);
    return;
    }
    }*/
}

void
GUIChildManager::scroll_up ()
{
  /*  for (auto i = m_components.begin (); i != m_components.end (); ++i)
      {
      if ((*i)->is_at (x - m_x, y - m_y))
      {
      (*i)->scroll_down (x - m_x, y - m_y);
      return;
      }
      }*/
}

void
GUIChildManager::scroll_down ()
{
  /*
    for (auto i = m_components.begin (); i != m_components.end (); ++i)
    {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
    (*i)->scroll_down (x - m_x, y - m_y);
    return;
    }
    }*/
}

void
GUIChildManager::on_mouse_move (int x, int y, int of_x, int of_y)
{
  GUIComponent* const comp = find_component_at (x, y);
  //std::cout << " MouseMove: " << x << " " << y << " " << comp << std::endl;

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
GUIChildManager::find_component_at (int x, int y)
{
  for (auto i = m_components.begin (); i != m_components.end (); ++i)
  {
    if ((*i)->is_at (x - m_x, y - m_y))
    {
      return *i;
    }
  }
  return nullptr;
}

/* EOF */
