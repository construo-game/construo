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

#include <geom/fwd.hpp>
#include <geom/point.hpp>

#include "fwd.hpp"

/** The GUIManager is basically the place where the main loop runs */
namespace construo {

class GUIManager
{
public:
  GUIManager ();
  virtual ~GUIManager ();

  /** @return the current frames per second */
  float get_fps() const { return m_current_fps; }

  /** Launches a single run from the games main loop */
  virtual void run_once(GraphicContext& gc);

  /** Draw all the GUI widgets */
  void draw(GraphicContext& gc);
  virtual void update() {}

  virtual void draw_overlay(GraphicContext& gc) {}

  virtual void set_geometry(geom::frect const& geometry) {}

  /** Stops the GUIManager */
  void quit();

  void add(std::unique_ptr<GUIWidget> widget);

  template<typename T, typename... Args>
  T* create(Args&&... args) {
    std::unique_ptr<T> obj = std::make_unique<T>(std::forward<Args>(args)...);
    T* ptr = obj.get();
    add(std::move(obj));
    return ptr;
  }

  void grab_mouse(GUIWidget& widget);
  void ungrab_mouse(GUIWidget& widget);

private:
  void process_events ();
  void process_button_events (ButtonEvent&);
  GUIWidget* find_widget_at(geom::fpoint const& pos) const;

private:
  unsigned int m_frame_count;
  unsigned long m_start_time;
  float m_current_fps;

  /** widget where the mouse is currently over */
  GUIWidget* m_last_widget;
  GUIWidget* m_current_widget;

  GUIWidget* m_grabbing_widget;

  geom::fpoint m_previous_pos;

  /** A collection of GUI widgets aka widgets */
  std::vector<std::unique_ptr<GUIWidget> > m_widgets;

public:
  GUIManager(const GUIManager&) = delete;
  GUIManager& operator=(const GUIManager&) = delete;
};

} // namespace construo

#endif

/* EOF */
