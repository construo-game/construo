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

#ifndef HEADER_ROOT_GRAPHIC_CONTEXT_HPP
#define HEADER_ROOT_GRAPHIC_CONTEXT_HPP

#include <stack>
#include "graphic_context.hpp"
#include "cursor_type.hpp"

/** RootGraphicContext represetens the window in which Construo runs,
 * it provides ways to set the title the cursor, etc. in addition to
 * the stuff that a normal GraphicContext provides.
 */
class RootGraphicContext : public GraphicContext
{
private:
  std::stack<CursorType> cursor_stack;
  CursorType current_cursor;
public:
  RootGraphicContext() {}
  virtual ~RootGraphicContext() {}

  void set_cursor(CursorType);
  void push_cursor();
  void pop_cursor();

protected:
  virtual void set_cursor_real(CursorType) =0;

public:
  /** Enter fullscreen mode */
  virtual void enter_fullscreen() =0;
  
  /** Leave fullscreen and return to windowed mode */
  virtual void leave_fullscreen() =0;

private:
  RootGraphicContext (const RootGraphicContext&);
  RootGraphicContext& operator= (const RootGraphicContext&);
};

#endif

/* EOF */
