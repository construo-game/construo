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

#ifndef HEADER_CONSTRUO_CONTROLLER_HXX
#define HEADER_CONSTRUO_CONTROLLER_HXX

#include "world.hxx"
#include "delta_manager.hxx"

/** Holds all the different World states, handles undo and things like that */
class Controller
{
private:
  std::vector<World*> undo_world_stack;
  std::vector<World*> redo_world_stack;

  bool running;
  bool slow_down;

  /** The current active world, don't delete this, delet is handled in
      the undo stacks */
  World* world;

  DeltaManager delta_manager;
  
  static Controller* instance_;
public:
  /** Create an empty world workspace */
  Controller ();
  Controller (const std::string& filename);
  ~Controller ();

  static inline Controller* instance() { return instance_; }

  bool has_been_run () { return world && world->get_has_been_run (); }
  
  bool is_running () { return running; }
  bool slow_down_active () { return slow_down; }
  void set_slow_down (bool s) { slow_down = s; }

  World* get_world () { assert(world); return world; }

  /** Load a world by name, name is relative to the construo user directory */
  void load_world (const std::string& name);

  /** Save a world by name, name is relative to the construo user directory */
  void save_world (const std::string& name);

  std::string get_slot_filename(int n);
  void save_to_slot (int n);
  void load_from_slot (int n);

  /** Updates the state of the controller and also the world */
  void update ();

  void clear_world ();

  void undo ();
  void redo ();

  /** start or stops the simulation */
  void start_simulation ();
};

#endif

/* EOF */
