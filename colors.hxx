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

#ifndef HEADER_CONSTRUO_COLORS_HXX
#define HEADER_CONSTRUO_COLORS_HXX

#include "color.hxx"

namespace Colors
{
  extern Color button_fg_pressed;
  extern Color button_fg_hover;
  extern Color button_fg_passive;

  extern Color button_bg_pressed;
  extern Color button_bg_hover;
  extern Color button_bg_passive;
  extern Color button_bg_active;

  /** Color for new springs and other helper lines */
  extern Color new_spring;

  extern Color selection_rect;
  extern Color selection_resizer;

  /** Color for collidables */
  extern Color rect_collider_fg;
  extern Color rect_collider_bg;

  /** Color for highlighted particles or springs */
  extern Color highlight;
}

#endif

/* EOF */
