// Construo - A wire-frame construction game
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

#include "colors.hpp"

namespace construo {

GUIStyle const g_style = {
  .button_fg_pressed = Color(0xFFFFFFFF),
  .button_fg_hover   = Color(0x009f9fFF),
  .button_fg_passive = Color(0x007f7fFF),

  .button_bg_pressed = Color(0x000000FF),
  .button_bg_hover   = Color(0x006060EE),
  .button_bg_passive = Color(0x004f4fAA),
  .button_bg_active  = Color(0x00AAAAAA),

  .new_spring        = Color(0xAAAAAAFF),

  .selection_rect    = Color(0xFFFFFFFF),
  .selection_resizer = Color(0xFFFF00FF),

  .rect_collider_fg  = Color(0x0000FFFF),
  .rect_collider_bg  = Color(0x0000AAFF),
  .ground_color      = Color(0x000044FF),
  .ground_grid_color = Color(0x000073FF),

  .highlight         = Color(0xEEEEEEFF),

  .grid_color        = Color(0x444444FF),
  .grid_color2       = Color(0x666666FF),

};

} // namespace construo

/* EOF */
