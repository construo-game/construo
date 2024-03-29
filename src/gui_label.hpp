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

#ifndef HEADER_CONSTRUO_GUI_LABEL_HPP
#define HEADER_CONSTRUO_GUI_LABEL_HPP

#include <functional>
#include <string>

#include "gui_widget.hpp"

namespace construo {

class GUILabel : public GUIWidget
{
public:
  GUILabel(std::string const& title);
  GUILabel(std::function<std::string ()> title_func);

  void draw(GraphicContext&) override;
  bool is_at(geom::fpoint const& pos) const override { return false; }

private:
  std::function<std::string ()> m_title_func;
};

} // namespace construo

#endif

/* EOF */
