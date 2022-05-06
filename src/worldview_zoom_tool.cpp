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
#include "worldview_component.hpp"
#include "worldview_zoom_tool.hpp"

WorldViewZoomTool::WorldViewZoomTool(WorldViewComponent& worldview) :
  WorldViewTool(worldview),
  m_click_pos(),
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
WorldViewZoomTool::draw_foreground (ZoomGraphicContext& gc)
{
  if (m_creating_zoom_rectangle)
  {
    float x = m_worldview.zoom().screen_to_world_x(g_input_context->get_mouse_x());
    float y = m_worldview.zoom().screen_to_world_y(g_input_context->get_mouse_y());

    gc.draw_rect(Math::min(x, m_click_pos.x),
                 Math::min(y, m_click_pos.y),
                 Math::max(x, m_click_pos.x),
                 Math::max(y, m_click_pos.y),
                 g_style.new_spring);
  }
}

void
WorldViewZoomTool::on_primary_button_press (float screen_x, float screen_y)
{
  m_creating_zoom_rectangle = true;
  m_click_pos.x = m_worldview.zoom().screen_to_world_x (screen_x);
  m_click_pos.y = m_worldview.zoom().screen_to_world_y (screen_y);
}

void
WorldViewZoomTool::on_primary_button_release (float screen_x, float screen_y)
{
  m_creating_zoom_rectangle = false;

  float x = m_worldview.zoom().screen_to_world_x (screen_x);
  float y = m_worldview.zoom().screen_to_world_y (screen_y);

  m_worldview.zoom().zoom_to(static_cast<int>(Math::min(x, m_click_pos.x)),
                                                 static_cast<int>(Math::min(y, m_click_pos.y)),
                                                 static_cast<int>(Math::max(x, m_click_pos.x)),
                                                 static_cast<int>(Math::max(y, m_click_pos.y)));
}

void
WorldViewZoomTool::on_secondary_button_press (float x, float y)
{
  m_worldview.zoom().zoom_out(x, y);
}

void
WorldViewZoomTool::on_secondary_button_release (float x, float y)
{

}

/* EOF */
