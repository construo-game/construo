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

#ifndef HEADER_CONSTRUO_RECT_COLLIDER_HXX
#define HEADER_CONSTRUO_RECT_COLLIDER_HXX

#include "collider.hxx"

/** */
class RectCollider : public Collider
{
private:
  // FIXME: Use the Rect class here
  float x1;
  float y1;
  float x2;
  float y2;

public:
  RectCollider (lisp_object_t* cursor);
  RectCollider (float x1_, float y1_, float x2_, float y2_);

  Collider* duplicate() const;

  Vector2d get_pos();
  void     set_pos(const Vector2d&);

  bool is_at (const Vector2d& pos);
  void bounce ();
  void draw (GraphicContext* gc);
  void draw_highlight (GraphicContext* gc);

  lisp_object_t* serialize();
};

#endif

/* EOF */
