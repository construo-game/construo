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

#include "controller.hpp"

#include <logmich/log.hpp>

#include "worldview_widget.hpp"
#include "construo_error.hpp"
#include "world_reader.hpp"
#include "world_writer.hpp"

namespace construo {

Controller* Controller::instance_ = nullptr;

Controller::Controller() :
  m_world(std::make_unique<World>()),
  m_undo_world_stack(),
  m_redo_world_stack(),
  m_running(false),
  m_slow_down(false),
  m_action_cam(false),
  m_hide_dots(false),
  m_particle_mass(0.1f),
  m_spring_stiffness(50.0f),
  m_delta_manager()
{
  assert(instance_ == nullptr);
  instance_ = this;
}

Controller::~Controller ()
{
  instance_ = nullptr;
}

void
Controller::load_world(const std::string& filename)
{
  std::unique_ptr<World> new_world;
  try {
    new_world = WorldReader().from_file(filename);
  } catch (std::exception const& err) {
    log_error("{}: error while loading: {}", filename, err.what());
    return;
  }

  m_undo_world_stack.push_back(std::move(m_world));

  m_world = std::move(new_world);

  if (WorldViewWidget::instance()) {
    WorldViewWidget::instance()->on_world_change();
  }

  m_running = false;
}

void
Controller::save_world (const std::string& filename)
{
  WorldWriter(*m_world).write(filename);
}

std::string
Controller::get_slot_filename(int slot)
{
  return "/user/" + fmt::format("quicksave{:d}.construo", slot);
}

void
Controller::save_to_slot (int n)
{
  try {
    save_world (get_slot_filename (n));
  } catch (std::exception const& err) {
    print_exception(err);
  }
}

void
Controller::load_from_slot (int n)
{
  try {
    load_world (get_slot_filename (n));
  } catch (std::exception const& err) {
    print_exception(err);
  }
}

void
Controller::update ()
{
  float delta = m_delta_manager.getset ();

  if (m_running)
    {
      float min_skip;

      if (m_slow_down)
        {
          delta /= 50.0f/20.0f;
          min_skip = 0.0007f;
        }
      else
        {
          delta /= 5.0f/20.0f;
          min_skip = 0.02f; // 0.02
        }

      float i = 0.0f;
      while (i < delta)
        {
          m_world->update (min_skip);
          i += min_skip;
        }
    }
}

void
Controller::start_simulation ()
{
  if (!m_running) {
    m_undo_world_stack.push_back(m_world->duplicate());
  }

  if (m_undo_world_stack.size() > 100)
    {
      // FIXME: shrink stack here
    }

  m_running = !m_running;
}

void
Controller::push_undo()
{
  m_undo_world_stack.push_back(m_world->duplicate());
}

void
Controller::clear_world ()
{
  m_undo_world_stack.push_back(std::move(m_world));
  m_world = std::make_unique<World>();
  m_running = false;
}

void
Controller::undo ()
{
  if (m_undo_world_stack.empty()) { return; }

  m_redo_world_stack.push_back(std::move(m_world));
  m_world = std::move(m_undo_world_stack.back());
  m_undo_world_stack.pop_back();
  m_running = false;
}

void
Controller::redo ()
{
  if (m_redo_world_stack.empty()) { return; }

  m_undo_world_stack.push_back(std::move(m_world));
  m_world = std::move(m_redo_world_stack.back());
  m_redo_world_stack.pop_back();
  m_running = false;
}

void
Controller::set_action_cam(bool a)
{
  m_action_cam = a;
}

bool
Controller::get_action_cam()
{
  return m_action_cam;
}

void
Controller::set_hide_dots (bool d)
{
  m_hide_dots = d;
}

bool
Controller::get_hide_dots ()
{
  return m_hide_dots;
}

} // namespace construo

/* EOF */
