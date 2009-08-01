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

#ifndef HEADER_CONSTRUO_CONSTRUO_HPP
#define HEADER_CONSTRUO_CONSTRUO_HPP

#include <config.h>

#define CONSTRUO_DATADIR DATADIR"/games/"PACKAGE

class SystemContext;
class InputContext;
class RootGraphicContext;

/** Global accessor to the system functions */
extern SystemContext*  system_context;

/** Global accessor to the input functions */
extern InputContext*   input_context;

/** Global accessor to the graphic device */
extern RootGraphicContext* graphic_context;

#endif

/* EOF */
