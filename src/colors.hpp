//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_CONSTRUO_COLORS_HPP
#define HEADER_CONSTRUO_COLORS_HPP

#include "color.hpp"

namespace Colors {

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
extern Color ground_color;
extern Color ground_grid_color;

/** Color for highlighted particles or springs */
extern Color highlight;

/** Color of the background grid */
extern Color grid_color;
extern Color grid_color2;

} // namespace Colors

#endif

/* EOF */
