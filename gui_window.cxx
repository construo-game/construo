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

#include "graphic_context.hxx"
#include "colors.hxx"
#include "gui_window.hxx"

GUIWindow::GUIWindow (const std::string& t, int x, int y, int width, int height)
  : GUIComponent (x, y, width, height),
    title (t)
{
}

GUIWindow::~GUIWindow ()
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      delete *i;
    }
}

void
GUIWindow::add (GUIComponent* comp)
{
  components.push_back(comp);
}

void
GUIWindow::draw (GraphicContext* parent_gc)
{
  gc.set_parent_gc (parent_gc);
  gc.set_offset (x_pos, y_pos + 25);
  
  parent_gc->draw_fill_rect (x_pos, y_pos,
                             x_pos + width, y_pos + height, 
                             Colors::button_bg_passive);
  parent_gc->draw_rect (x_pos, y_pos,
                        x_pos + width, y_pos + height, 
                        Colors::button_fg_passive);

  parent_gc->draw_fill_rect (x_pos, y_pos,
                             x_pos + width, y_pos + 25, 
                             Colors::button_bg_hover);

  parent_gc->draw_rect (x_pos, y_pos,
                        x_pos + width, y_pos + 25, 
                        Colors::button_fg_passive);

  parent_gc->draw_string (x_pos + 10, y_pos + 16, title);

  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      (*i)->draw (&gc);
    }
}

void
GUIWindow::on_primary_button_press (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_primary_button_press (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::on_primary_button_release (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_primary_button_release (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::on_secondary_button_press (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_secondary_button_press (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::on_secondary_button_release (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_secondary_button_release (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}
  
void
GUIWindow::on_delete_press (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_delete_press (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::on_fix_press (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->on_fix_press (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::on_mouse_enter ()
{
}

void
GUIWindow::on_mouse_leave ()
{
}

void
GUIWindow::wheel_up (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->wheel_up (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::wheel_down (int x, int y)
{
  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
        {
          (*i)->wheel_down (x - x_pos, y - y_pos - 25);
          return;
        }
    }
}

void
GUIWindow::scroll_left ()
{
  /*  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
      {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
      {
      (*i)->scroll_left (x - x_pos, y - y_pos - 25);
      return;
      }
      }*/
}

void
GUIWindow::scroll_right ()
{
  /*
    for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
    if ((*i)->is_at (x - x_pos, y - y_pos - 25))
    {
    (*i)->scroll_right (x - x_pos, y - y_pos - 25);
    return;
    }
    }*/
}

void
GUIWindow::scroll_up ()
{
  /*  for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
      {
      if ((*i)->is_at (x - x_pos, y - y_pos - 25))
      {
      (*i)->scroll_down (x - x_pos, y - y_pos - 25);
      return;
      }
      }*/
}

void
GUIWindow::scroll_down ()
{
  /*
    for (ComponentLst::iterator i = components.begin (); i != components.end (); ++i)
    {
    if ((*i)->is_at (x - x_pos, y - y_pos - 25))
    {
    (*i)->scroll_down (x - x_pos, y - y_pos - 25);
    return;
    }
    }*/
}

void
GUIWindow::on_mouse_move (int x, int y, int of_x, int of_y)
{
}

/* EOF */
