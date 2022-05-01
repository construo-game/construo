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

  /** Stops the GUIManager */
  void quit();

  /** add a GUIComponent, the component will get deleted after in the
   *  destruction of the guimanager */
  void add (GUIComponent*);

  void grab_mouse (GUIComponent*);
  void ungrab_mouse (GUIComponent*);

private:
  void process_events ();
  void process_button_events (ButtonEvent&);
  GUIComponent* find_component_at (int, int);

private:
  unsigned int m_frame_count;
  unsigned int m_start_time;
  float m_current_fps;

  /** component where the mouse is currently over */
  GUIComponent* m_last_component;
  GUIComponent* m_current_component;

  GUIComponent* m_grabbing_component;

  int m_last_x;
  int m_last_y;

  /** A collection of GUI components aka widgets */
  std::vector<GUIComponent*> m_components;

public:
  GUIManager(const GUIManager&) = delete;
  GUIManager& operator=(const GUIManager&) = delete;
};

#endif

/* EOF */
