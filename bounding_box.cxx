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

#include "vector2d.hxx"
#include "math.hxx"
#include "bounding_box.hxx"

BoundingBox::BoundingBox()
  : x1(0), y1(0), x2(0), y2(0)
{
}

BoundingBox::BoundingBox(float x1_, float y1_, float x2_, float y2_)
  : x1(x1_), y1(y1_), x2(x2_), y2(y2_)
{
}

void
BoundingBox::join(const BoundingBox& box)
{
  x1 = Math::min(x1, box.x1);
  y1 = Math::min(y1, box.y1);

  x2 = Math::max(x2, box.x2);
  y2 = Math::max(y2, box.y2);
}

void
BoundingBox::join(const Vector2d& pos)
{
  x1 = Math::min(x1, pos.x);
  y1 = Math::min(y1, pos.y);

  x2 = Math::max(x2, pos.x);
  y2 = Math::max(y2, pos.y);
}

std::ostream& operator << (std::ostream& os, const BoundingBox& box)
{
  return os << "[BB:" 
            << box.x1 << ", " << box.y1 << ", " << box.x2 << ", " << box.y2
            << "]";
}

/* EOF */
