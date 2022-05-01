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

#include "world.hpp"
#include "controller.hpp"
#include "screen_manager.hpp"
#include "world_button.hpp"
#include "construo_error.hpp"

WorldButton::WorldButton (const std::string& arg_filename, Mode m) :
  GUIFileButton(arg_filename),
  m_world(nullptr),
  m_file_broken(false),
  m_mtime(),
  m_mode(m)
{
}

WorldButton::~WorldButton ()
{
  delete m_world;
}

void
WorldButton::load_world ()
{
  if ((m_world == nullptr
       && !m_file_broken)
      || m_mtime != g_system_context->get_mtime(m_filename))
    {
      try {
        delete m_world;
        m_world = new World(m_filename);
        m_mtime = g_system_context->get_mtime(m_filename);
      } catch (std::exception const& err) {
        print_exception(err);
        std::cout << "ERROR: WorldButton: Somthing went wrong loading " << m_filename << std::endl;
        m_world = nullptr;
        m_file_broken = true;
      }
    }
}

void
WorldButton::draw (GraphicContext* parent_gc)
{
  load_world();

  parent_gc->draw_fill_rect(static_cast<float>(m_x),
                            static_cast<float>(m_y),
                            static_cast<float>(m_x + m_width),
                            static_cast<float>(m_y + m_height),
                            Color (0xBB0000FF));

  ZoomGraphicContext gc(m_x, m_y, m_x + m_width, m_y + m_height);
  gc.set_parent_gc(parent_gc);

  gc.lock();

  if (m_world)
  {
    // FIXME: bounding box should be calculated in construtor
    const BoundingBox& box = m_world->calc_bounding_box();
    gc.zoom_to(static_cast<int>(box.x1), static_cast<int>(box.y1),
               static_cast<int>(box.x2),  static_cast<int>(box.y2));
    m_world->draw_colliders (&gc);
    m_world->draw_springs (&gc);
  }
  else
  {
    // Draw an 'X' for broken levels
    gc.draw_line(0, 0, static_cast<float>(gc.get_width()), static_cast<float>(gc.get_height()), Color (0xFF00FFFF));
    gc.draw_line(0, static_cast<float>(gc.get_height()), static_cast<float>(gc.get_width()), 0, Color (0xFF00FFFF));
  }

  gc.unlock();

  if (m_mouse_over)
    parent_gc->draw_rect(static_cast<float>(m_x),
                         static_cast<float>(m_y),
                         static_cast<float>(m_x +  m_width),
                         static_cast<float>(m_y + m_height),
                         Color (0xFFFFFFFF));
  else
    parent_gc->draw_rect(static_cast<float>(m_x),
                         static_cast<float>(m_y),
                         static_cast<float>(m_x + m_width),
                         static_cast<float>(m_y + m_height),
                         Color (0xFF0000FF));

  parent_gc->draw_string(static_cast<float>(m_x + 20),
                         static_cast<float>(m_y + 160),
                         m_filename);
}

void
WorldButton::on_click ()
{
  //std::cout << "WorldButton: detected click on: " << filename << std::endl;
  if (m_mode == SAVE_BUTTON)
    {
      Controller::instance()->save_world(m_filename);
      ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
    }
  else // LOAD BUTTON
    {
      Controller::instance()->load_world(m_filename);
      ScreenManager::instance()->set_gui(ScreenManager::WORLD_GUI);
    }
}

/* EOF */
