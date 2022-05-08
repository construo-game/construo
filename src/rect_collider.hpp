// Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_RECT_COLLIDER_HPP
#define HEADER_CONSTRUO_RECT_COLLIDER_HPP

#include "collider.hpp"

class RectCollider : public Collider
{
public:
  RectCollider(ReaderMapping const& reader);
  RectCollider(float x1_, float y1_, float x2_, float y2_);

  Collider* duplicate() const override;

  glm::vec2 get_pos() override;
  void     set_pos(const glm::vec2&) override;

  bool is_at (const glm::vec2& pos) override;
  void bounce() override;
  void draw (GraphicContext& gc) override;
  void draw_highlight (GraphicContext& gc) override;

  BoundingBox get_bounding_box() const override;

  void serialize(prio::Writer& writer) override;

private:
  // FIXME: Use the Rect class here
  float x1;                     /**< left border */
  float y1;                     /**< top border */
  float x2;                     /**< right border */
  float y2;                     /**< bottom border */
};

#endif

/* EOF */
