//  $Id$
// 
//  Construo - A wire-frame construction game
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

#include "root_graphic_context.hxx"

void
RootGraphicContext::set_cursor(CursorType cursor)
{
  current_cursor = cursor;
  set_cursor_real(cursor);
}

void
RootGraphicContext::push_cursor()
{
  cursor_stack.push(current_cursor);
}

void
RootGraphicContext::pop_cursor()
{
  if (!cursor_stack.empty())
    {
      set_cursor(cursor_stack.top());
      cursor_stack.pop();
    }
  else
    {
      std::cout << "RootGraphicContext::pop_cursor(): Stack empty!" << std::endl;
    }
}

/* EOF */
