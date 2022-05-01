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

#include "worldview_component.hpp"
#include "construo_error.hpp"
#include "controller.hpp"

Controller* Controller::instance_ = nullptr;

Controller::Controller() :
  m_world(new World),
  m_undo_world_stack(),
  m_redo_world_stack(),
  m_running(false),
  m_slow_down(false),
  m_action_cam(false),
  m_hide_dots(false),
  m_delta_manager()
{
  instance_ = this;
}

Controller::Controller(const std::string& filename) :
  m_world(new World(filename)),
  m_undo_world_stack(),
  m_redo_world_stack(),
  m_running(false),
  m_slow_down(false),
  m_action_cam(false),
  m_hide_dots(false),
  m_delta_manager()
{
  instance_ = this;
}

Controller::~Controller ()
{
  instance_ = nullptr;
}

void
Controller::load_world(const std::string& filename)
{
  if (m_world) {
    m_undo_world_stack.push_back(m_world);
  }

  //std::cout << "Loading World..." << std::endl;
  m_world = new World (filename);

  WorldViewComponent::instance()->on_world_change();

  m_running = false;
  //std::cout << "Loading World... DONE" << std::endl;
}

void
Controller::save_world (const std::string& filename)
{
  //std::cout << "Saving World..." << std::endl;
  m_world->write_lisp (filename);
  //std::cout << "Saving World... DONE" << std::endl;
}


std::string
Controller::get_slot_filename(int n)
{
  return "/user/" + std::string("quicksave") + char('0' + n) + ".construo";
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
      //delete *undo_world_stack.front();
      //std::cout << "Stak
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
  std::cout << "Controller: Clear" << std::endl;
  m_undo_world_stack.push_back(m_world);
  m_world = new World;
  m_running = false;
}

void
Controller::undo ()
{
#ifdef DEBUG
  std::cout << "Controller::undo (): undostack: " << undo_world_stack.size()
            << " redostack: " << redo_world_stack.size() << std::endl;
#endif

  if (!m_undo_world_stack.empty())
    {
      //delete world; // fixme: memory hole
      m_redo_world_stack.push_back(m_world);
      m_world = m_undo_world_stack.back();
      m_undo_world_stack.pop_back();
      m_running = false;
    }
  else
    {
      std::cout << "Undo stack empty" << std::endl;
    }
}

void
Controller::redo ()
{
  if (!m_redo_world_stack.empty())
    {
      m_undo_world_stack.push_back(m_world);
      m_world = m_redo_world_stack.back();
      m_redo_world_stack.pop_back();
      m_running = false;
    }
  else
    {
      std::cout << "Redo stack empty" << std::endl;
    }
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

/* EOF */
