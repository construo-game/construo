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

#ifndef HEADER_X11_DISPLAY_HXX
#define HEADER_X11_DISPLAY_HXX

#include <X11/Xlib.h>

/** X11Display driver */
class X11Display : public GraphicContext,
                   public InputContext,
                   public SystemContext
{
private:
  int       width;
  int       height;
  Display*  display;
  Window    window;
  GC        gc;

  int mouse_x;
  int mouse_y;
public:
  X11Display(int w, int h);

  void draw_line(int x1, int y1, int x2, int y2, int color = 0xFFFFFF, int wide = 0);
  void draw_filled_rect(int x1, int y1, int x2, int y2, int color = 0xFFFFFF);
private:
  X11Display (const X11Display&);
  X11Display& operator= (const X11Display&);
};

#endif

/* EOF */
