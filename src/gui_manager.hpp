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

#ifndef HEADER_CONSTRUO_GUI_MANAGER_HPP
#define HEADER_CONSTRUO_GUI_MANAGER_HPP

#include <memory>
#include <vector>

class GUIComponent;
struct ButtonEvent;

/** The GUIManager is basically the place where the main loop runs */
class GUIManager
{
public:
  GUIManager ();
  virtual ~GUIManager ();

  /** @return the current frames per second */
  float get_fps() const { return m_current_fps; }

  /** Launches a single run from the games main loop */
  virtual void run_once ();

  /** Draw all the GUI components */
  void draw ();
  virtual void update() {}

  virtual void draw_overlay () {}

  virtual void resize(int width, int height) {}

  /** Stops the GUIManager */
  void quit();

  void add(std::unique_ptr<GUIComponent> component);

  template<typename T, typename... Args>
  T* create(Args&&... args) {
    std::unique_ptr<T> obj = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = obj.get();
    add(std::move(obj));
    return ptr;
  }

  void grab_mouse (GUIComponent*);
  void ungrab_mouse (GUIComponent*);

private:
  void process_events ();
  void process_button_events (ButtonEvent&);
  GUIComponent* find_component_at(float x, float y);

private:
  unsigned int m_frame_count;
  unsigned int m_start_time;
  float m_current_fps;

  /** component where the mouse is currently over */
  GUIComponent* m_last_component;
  GUIComponent* m_current_component;

  GUIComponent* m_grabbing_component;

  float m_last_x;
  float m_last_y;

  /** A collection of GUI components aka widgets */
  std::vector<std::unique_ptr<GUIComponent> > m_components;

public:
  GUIManager(const GUIManager&) = delete;
  GUIManager& operator=(const GUIManager&) = delete;
};

#endif

/* EOF */
