//  $Id$
// 
//  Pingus - A free Lemmings clone
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

#ifndef HEADER_CONSTRUO_RECT_COLLIDER_HXX
#define HEADER_CONSTRUO_RECT_COLLIDER_HXX

#include "collider.hxx"

/** */
class RectCollider : public Collider
{
private:
  int x1;
  int y1;
  int x2;
  int y2;

public:
  RectCollider (int x1_, int y1_, int x2_, int y2_);

  bool is_at (const Vector2d& pos);
  void bounce (Particle& particle);
  void draw (GraphicContext* gc);
};

#endif

/* EOF */
