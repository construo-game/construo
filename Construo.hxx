//  $Id$
// 
//  Pingus - A free Lemmings clone
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <strstream>
#include <libxml/parser.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <ClanLib/application.h>
#include "Config.hxx"

const double back_force = 500.0;
const double max_stretch = 0.1;

std::string to_xml (const CL_Vector& vec)
{
  std::ostrstream out;
  out << "<cl-vector>"
      << "<x>" << vec.x << "</x>"
      << "<y>" << vec.y << "</y>"
      << "<z>" << vec.z << "</z>"
      << "</cl-vector>" << std::ends;
  std::string str (out.str ());
  out.freeze (false);
  return str;
}

float to_float(xmlDocPtr doc, xmlNodePtr cur)
{
  assert (cur && doc);
  if (cur)
    {
      char* ident = (char*)xmlNodeListGetString(doc, cur, 1);
      float i;
      if (sscanf(ident, "%f", &i) != 1)
	std::cout << "Not a float" << std::endl;
      free (ident);
      return i;
    }
  else
    {
      std::cout << "Node empty" << std::endl;
      return 0;
    }
}

int to_int(xmlDocPtr doc, xmlNodePtr cur)
{
  assert (cur && doc);
  if (cur)
    {
      char* ident = (char*)xmlNodeListGetString(doc, cur, 1);
      int i;
      if (sscanf(ident, "%d", &i) != 1)
	std::cout << "Not a int" << std::endl;
      std::cout << "int: " << i << std::endl;
      free (ident);
      return i;
    }
  else
    {
      std::cout << "Node empty" << std::endl;
      return 0;
    }
}

CL_Vector cl_vector_from_xml (xmlDocPtr doc, xmlNodePtr cur)
{
  std::cout << "parsing vector" << std::endl;
  CL_Vector pos;

  assert (cur);

  if (xmlIsBlankNode(cur)) {
    cur = cur->next;
  }
  cur = cur->children;
  while (cur)
    {
      if (xmlIsBlankNode(cur)) {
	cur = cur->next;
	continue;
      }

      if (strcmp((char*)cur->name, "x") == 0) {
	pos.x = to_float(doc, cur->children);
      } else if (strcmp((char*)cur->name, "y") == 0) {
	pos.y = to_float(doc, cur->children);
      } else if (strcmp((char*)cur->name, "z") == 0) {
	pos.z = to_float(doc, cur->children);
      } else {
	std::cout << "cl_vector_from_xml: Unknown ident: " << (char*)cur->name << std::endl;
      }
      cur = cur->next;
    }
  return pos;
}

class Particle
{
public:
  static unsigned id_counter;
  unsigned int id;
  /// position of the particle
  CL_Vector pos;

  /// velocity of the particle
  CL_Vector  velocity;

  /// totale force acting on particle
  CL_Vector totale_force;

  // the mass of the particle as 1/mass
  float mass;

  Particle (CL_Vector arg_pos, CL_Vector arg_velocity)
    : id (++id_counter), pos (arg_pos),
      velocity (arg_velocity)
  {
    mass = 10.0;
  }

  void set_id (unsigned int arg_id)
  {
    if (arg_id > id_counter)
      id_counter = arg_id;
    id = arg_id;
  }

  unsigned int get_id ()
  {
    return id;
  }

  void write_xml (ostream& out)
  {
    out << "    <particle id=\"" << id << "\">"
      	<< "<position>" << to_xml (pos) << "</position>"
	<< "<velocity>" << to_xml (velocity) << "</velocity>"
	<< "</particle>" << std::endl;
  }

  void add_force (CL_Vector force)
  {
    totale_force += force;
  }

  void clear_force ()
  {
    totale_force = CL_Vector ();
  }

  void update (float delta) 
  {
    velocity += totale_force * mass * delta;

    pos += velocity * delta;

    float damp = 0.2;

    if (pos.x < 0) {
      velocity.x =  fabs(velocity.x);
      pos.x = 0;
      velocity *= damp;
    } else if (pos.x > 800) {
      velocity.x =  -fabs(velocity.x);
      pos.x = 800;
      velocity *= damp;
    }

    if (pos.y < 0) {
      velocity.y =  fabs(velocity.y);
      pos.y = 0;
      velocity *= damp;
    } else if (pos.y > 600) {
      velocity.y =  -fabs(velocity.y);
      pos.y = 600;
      velocity *= damp;
    }

    /*
    CL_Vector dist = pos - CL_Vector (400, 300);
    if (dist.norm () < 50.0f)
      {
	velocity = -velocity;
      }*/
  }

  void draw () 
  {
    int size = int(10.0f/(mass*mass)) + 1;
        
    CL_Display::fill_rect (int(pos.x - size), int(pos.y - size),
			   int(pos.x + size), int(pos.y + size), 1.0f, 0.0f, 0.0f);
  }
};

class Stick
{
public:
  std::pair<Particle*, Particle*> particles;
  
  float length;
  bool destroyed;

  Stick (Particle* f, Particle* s) {
    particles.first = f;
    particles.second = s;
    destroyed = false;
    length = fabs((f->pos - s->pos).norm ());
  }

  void write_xml (ostream& out)
  {
    out << "    <spring>"
	<< "<first>" << particles.first->get_id () << "</first>"
	<< "<second>" << particles.second->get_id () << "</second>"
	<< "</spring>" << std::endl;
  }
  
  void update (float delta)
  {
    CL_Vector dist = particles.first->pos - particles.second->pos;
    float stretch = dist.norm ()/length - 1.0f;
    stretch *= 2.0f; // Materialkoeffizent
    //std::cout << "stretch: " << stretch << std::endl;

    if (fabs(stretch) > max_stretch)
      {
	destroyed = true;
      }
    else
      {
	dist.normalize ();
	CL_Vector force = dist * stretch * back_force;
	//std::cout << "Force: " << force << std::endl;
	particles.first->add_force (-force);
	particles.second->add_force (force);
      }
  }
  
  void draw ()
  {
    CL_Vector dist = particles.first->pos - particles.second->pos;
    float stretch = fabs(dist.norm ()/length - 1.0f) * 10.0f;

    float color = fabs((stretch/max_stretch));

    CL_Display::draw_line (int(particles.first->pos.x), int(particles.first->pos.y),
			   int(particles.second->pos.x), int(particles.second->pos.y),
			   color, 
			   1.0f - color, 
			   0.0f);
  }
};


class Construo : public CL_ClanApplication
{
private:
  CL_Slot slot_press;
  CL_Slot slot_release;

  CL_Vector click_pos;

  std::list<Particle*> particles;
  typedef std::list<Particle*>::iterator ParticleIter;

  std::list<Stick*> sticks;
  typedef std::list<Stick*>::iterator StickIter;

  void load_particles (xmlDocPtr doc, xmlNodePtr arg_cur);
  void load_springs (xmlDocPtr doc, xmlNodePtr arg_cur);
  Particle* last_particle;

  Particle* c_particle;
  bool running;
  bool slow_down;
  Config config;
public:
  Construo ();
  virtual ~Construo ();

  char* get_title ();
  int main (int argc, char* argv[]);

  void zero_out_velocity ();
  void load_or_save_xml (std::string filename);
  void load_xml (std::string filename);
  void save_xml (std::string filename);

  Particle* id_to_particle (unsigned int i);

  void on_press(CL_InputDevice *device, const CL_Key &key);
  void on_release(CL_InputDevice *device, const CL_Key &key);

  Particle* current_particle ();

} app;

#endif

/* EOF */
