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

#ifndef CONSTRUO_HH
#define CONSTRUO_HH

#include <vector>
#include <iostream>
#include <list>
#include "stick.hxx"
#include "particle.hxx"
#include "config.hxx"

class World;

class ConstruoMain
{
private:
  std::vector<World*> world_stack;
  CL_Vector click_pos;

  void load_particles (xmlDocPtr doc, xmlNodePtr arg_cur);
  void load_springs (xmlDocPtr doc, xmlNodePtr arg_cur);
  Particle* last_particle;

  Particle* c_particle;
  bool running;
  bool slow_down;
  bool do_quit;
  Config config;

  World* world;
public:
  ConstruoMain ();
  virtual ~ConstruoMain ();

  char* get_title ();
  int main (int argc, char* argv[]);

  void zero_out_velocity ();
  void load_or_save_xml (std::string filename);
  void load_xml (std::string filename);
  void save_xml (std::string filename);

  Particle* id_to_particle (unsigned int i);

  void on_mouse_press(int button_no);
  void on_mouse_release(int button_no);

  void on_key_press(int key_id);
  void on_key_release(int key_id);

  Particle* current_particle ();

private:
  void process_events ();
};

#endif

/* EOF */
