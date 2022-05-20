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

#include "colors.hpp"
#include "construo.hpp"
#include "input_context.hpp"
#include "math.hpp"
#include "worldview_widget.hpp"
#include "worldview_zoom_tool.hpp"

namespace construo {

WorldViewZoomTool::WorldViewZoomTool(WorldViewWidget& worldview) :
  WorldViewTool(worldview),
  m_click_pos(0.0f, 0.0f),
  m_creating_zoom_rectangle(false)
{
}

WorldViewZoomTool::~WorldViewZoomTool()
{
}

void
WorldViewZoomTool::activate ()
{
  m_creating_zoom_rectangle = false;
}

void
WorldViewZoomTool::draw_foreground(ZoomGraphicContext& gc)
{
  if (m_creating_zoom_rectangle)
  {
    geom::fpoint const pos = m_worldview.zoom().screen_to_world(g_input_context->get_mouse_pos());

    gc.draw_rect(geom::normalize(geom::frect(pos, m_click_pos)), g_style.selection_rect);
  }
}

void
WorldViewZoomTool::on_primary_button_press(geom::fpoint const& screen_pos)
{
  m_creating_zoom_rectangle = true;

  m_click_pos = m_worldview.zoom().screen_to_world(screen_pos);
}

void
WorldViewZoomTool::on_primary_button_release(geom::fpoint const& screen_pos)
{
  m_creating_zoom_rectangle = false;

  geom::fpoint const pos = m_worldview.zoom().screen_to_world(screen_pos);

  m_worldview.zoom().zoom_to(geom::frect(std::min(pos.x(), m_click_pos.x()),
                                         std::min(pos.y(), m_click_pos.y()),
                                         std::max(pos.x(), m_click_pos.x()),
                                         std::max(pos.y(), m_click_pos.y())));
}

void
WorldViewZoomTool::on_secondary_button_press(geom::fpoint const& pos)
{
  m_worldview.zoom().zoom_out(pos);
}

void
WorldViewZoomTool::on_secondary_button_release(geom::fpoint const& pos)
{

}

} // namespace construo

/* EOF */
