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

#include "widget_group.hpp"

#include <algorithm>

#include "graphic_context.hpp"
#include "colors.hpp"

namespace construo {

WidgetGroup::WidgetGroup() :
  GUIWidget(),
  m_widgets(),
  m_current_widget(nullptr)
{
}

WidgetGroup::~WidgetGroup ()
{
}

void
WidgetGroup::add(std::unique_ptr<GUIWidget> comp)
{
  m_widgets.push_back(std::move(comp));
}

void
WidgetGroup::remove(GUIWidget* comp)
{
  std::erase_if(m_widgets, [comp](auto&& item){
    return item.get() == comp;
  });

  if (m_current_widget == comp) {
    m_current_widget = nullptr;
  }
}

void
WidgetGroup::clear()
{
  m_widgets.clear();
  m_current_widget = nullptr;
}

void
WidgetGroup::draw(GraphicContext& parent_gc)
{
  GCZoomState zoom(geom::frect(parent_gc.geometry().size()));
  zoom.set_offset(m_geometry.topleft());

  ZoomGraphicContext gc(parent_gc, zoom);

  parent_gc.draw_fill_rect(m_geometry, g_style.button_bg_passive);
  parent_gc.draw_rect(m_geometry, g_style.button_fg_passive);

  parent_gc.draw_fill_rect(m_geometry, g_style.button_bg_hover);
  parent_gc.draw_rect(m_geometry, g_style.button_fg_passive);

  for (auto i = m_widgets.rbegin(); i != m_widgets.rend(); ++i)
  {
    (*i)->draw(gc);
  }

  draw_overlay(parent_gc);
}

void
WidgetGroup::on_primary_button_press(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_primary_button_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_primary_button_release(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_primary_button_release(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_secondary_button_press(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_secondary_button_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_secondary_button_release(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_secondary_button_release(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_delete_press(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_delete_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_fix_press(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->on_fix_press(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::on_mouse_enter()
{
}

void
WidgetGroup::on_mouse_leave()
{
}

void
WidgetGroup::wheel_up(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->wheel_up(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::wheel_down(geom::fpoint const& pos)
{
  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
  {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
      (*i)->wheel_down(pos - geom::foffset(m_geometry.topleft()));
      return;
    }
  }
}

void
WidgetGroup::scroll_left()
{
  /*  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
      {
      if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
      {
      (*i)->scroll_left(pos - geom::foffset(m_geometry.topleft()));
      return;
      }
      }*/
}

void
WidgetGroup::scroll_right()
{
  /*
    for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
    {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
    (*i)->scroll_right(pos - geom::foffset(m_geometry.topleft()));
    return;
    }
    }*/
}

void
WidgetGroup::scroll_up()
{
  /*  for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
      {
      if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
      {
      (*i)->scroll_down(pos - geom::foffset(m_geometry.topleft()));
      return;
      }
      }*/
}

void
WidgetGroup::scroll_down()
{
  /*
    for (auto i = m_widgets.begin(); i != m_widgets.end(); ++i)
    {
    if ((*i)->is_at(pos - geom::foffset(m_geometry.topleft())))
    {
    (*i)->scroll_down(pos - geom::foffset(m_geometry.topleft()));
    return;
    }
    }*/
}

void
WidgetGroup::on_mouse_move(geom::fpoint const& pos, geom::foffset const& offset)
{
  GUIWidget* const comp = find_widget_at(pos);

  if (comp != m_current_widget)
  {
    if (comp) {
      comp->on_mouse_enter();
    }
    if (m_current_widget) {
      m_current_widget->on_mouse_leave();
    }
    m_current_widget = comp;
  }
  else if (comp)
  {
    comp->on_mouse_move(pos, offset);
  }
}

GUIWidget*
WidgetGroup::find_widget_at(geom::fpoint const& pos) const
{
  for (auto& widget : m_widgets)
  {
    if (widget->is_at(pos - geom::foffset(m_geometry.topleft()))) {
      return widget.get();
    }
  }
  return nullptr;
}

} // namespace construo

/* EOF */
