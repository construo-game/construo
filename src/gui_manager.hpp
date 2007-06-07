//  $Id$
// 
//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 2
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
// 
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

#ifndef HEADER_CONSTRUO_GUI_MANAGER_HPP
#define HEADER_CONSTRUO_GUI_MANAGER_HPP

#include <vector>

class GUIComponent;
struct ButtonEvent;

/** The GUIManager is basically the place where the main loop runs */
class GUIManager
{
private:
  unsigned int frame_count;
  unsigned int start_time;
  float current_fps;

  /** component where the mouse is currently over */
  GUIComponent* last_component;
  GUIComponent* current_component;

  GUIComponent* grabbing_component;
  
  int last_x;
  int last_y;

  /** A collection of GUI components aka widgets */
  typedef std::vector<GUIComponent*> ComponentLst;
  ComponentLst components;

  void process_events ();
  void process_button_events (ButtonEvent&);
  GUIComponent* find_component_at (int, int);
public:
  GUIManager ();
  virtual ~GUIManager ();

  /** @return the current frames per second */
  float get_fps() const { return current_fps; }
  
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
};

#endif

/* EOF */
