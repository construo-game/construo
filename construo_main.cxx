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

#include <fstream>
#include "delta_manager.hxx"
#include "construo.hxx"
#include "particle.hxx"
#include "construo_main.hxx"
#include "config.h"

#include "x11_display.hxx"
#include "unix_system.hxx"

unsigned int Particle::id_counter;

ConstruoMain::ConstruoMain ()
{
  last_particle = 0;
  running = false;
  c_particle = 0;
  load_xml (config.get_construo_dir () + "/" + "quicksave1.xml");
}

ConstruoMain::~ConstruoMain ()
{
}

char* 
ConstruoMain::get_title ()
{
  return "Construo";
}

void 
ConstruoMain::on_mouse_press(int button_no) 
{
#if 0 
  click_pos = CL_Vector (key.x, key.y);

  if (c_particle)
    {
      Particle* particle = current_particle ();
      if (!particle)
        {
          particle = new Particle (click_pos,
                                   CL_Vector(0,0));
          particles.push_back (particle);
        }
      if (c_particle != particle)
        sticks.push_back (new Stick (c_particle, particle));
      c_particle = 0;
    }
  else
    {
      c_particle = current_particle ();
      if (!c_particle)
        {
          c_particle = new Particle (click_pos,
                                     CL_Vector(0,0));
          particles.push_back (c_particle);
        }
    }
#endif
}

void 
ConstruoMain::on_key_press(int key_id)
{
#if 0
  else if (device == CL_Input::keyboards[0])
    {
      switch (key.id) {
      case CL_KEY_SPACE:
	running = !running;
	break;
      case CL_KEY_C:
	particles.clear ();
	sticks.clear ();
	break;
      case CL_KEY_S:
	{
	  std::string filename;
	  std::cout << "Please input filename for saving: " << std::flush;
	  std::cin >> filename;
	  save_xml (config.get_construo_dir () + "/" + filename);
	}
	break;
      case CL_KEY_L:
	{
	  std::string filename;
	  std::cout << "Please input filename for loading: " << std::flush;
	  std::cin >> filename;
	  load_xml (config.get_construo_dir () + "/" + filename);
	}
	break;
      case CL_KEY_V:
	zero_out_velocity ();
	break;
      case CL_KEY_D:
	slow_down = !slow_down;
	break;
      case CL_KEY_1:
	load_or_save_xml (config.get_construo_dir () + "/" + "quicksave1.xml");
	break;
      case CL_KEY_2:
	load_or_save_xml (config.get_construo_dir () + "/" + "quicksave2.xml");
	break;
      case CL_KEY_3:
	load_or_save_xml (config.get_construo_dir () + "/" + "quicksave3.xml");
	break;
      case CL_KEY_4:
	load_or_save_xml (config.get_construo_dir () + "/" + "quicksave4.xml");
	break;	
      }
    }
#endif
}

void
ConstruoMain::load_or_save_xml (std::string filename)
{
#if 0
  if (CL_Keyboard::get_keycode (CL_KEY_LSHIFT)
      || CL_Keyboard::get_keycode (CL_KEY_RSHIFT))
    load_xml (filename);
  else
    save_xml (filename);
#endif
}

void
ConstruoMain::load_particles (xmlDocPtr doc, xmlNodePtr arg_cur)
{
#if 0
  xmlNodePtr cur = arg_cur->children;

  while (cur != NULL)
    {  
      if (xmlIsBlankNode(cur)) {
	cur = cur->next;
        continue;
      }
  
      if (strcmp((char*)cur->name, "particle") == 0)
	{
	  std::cout << "Found particle" << std::endl;
	  CL_Vector velocity, position;

	  int id;
	  
	  char* count = (char*)xmlGetProp(cur, (xmlChar*)"id");
	  if (count) {
	    sscanf (count, "%d", &id);
	    free (count);
	    std::cout << "Found id: " << id << std::endl;
	  } else {
	    std::cout << "id missing: " << id << std::endl;
	  }	  

	  xmlNodePtr ccur = cur->children;
	  while (ccur != NULL)
	    {	
	      if (xmlIsBlankNode(ccur)) {
		ccur = ccur->next;
		continue;
	      }

	      if (strcmp((char*)ccur->name, "velocity") == 0)
		{
		  velocity = cl_vector_from_xml (doc, ccur->children);
		}
	      else if (strcmp((char*)ccur->name, "position") == 0)
		{
		  position = cl_vector_from_xml (doc, ccur->children);
		}
	      else
		{
		  std::cout << "Unknown ident: " << (char*)ccur->name << std::endl;
		}
	      ccur = ccur->next;
	    }
	  std::cout << "Creating particle" << std::endl;
	  Particle* p = new Particle (position, velocity);
	  p->set_id (id);
	  particles.push_back (p);
	  std::cout << "Creating particle done" << std::endl;
	}
      else
	{
	  std::cout << "load_particles: Unknown element: " << (char*)cur->name << std::endl;
	}

      cur = cur->next;
    }
#endif
}

Particle* 
ConstruoMain::id_to_particle (unsigned int n)
{
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      if ((*i)->get_id () == n)
	return (*i);
    }
  std::cout << "Couldn't find particle id" << std::endl;
  return 0;
}

void
ConstruoMain::load_springs (xmlDocPtr doc, xmlNodePtr arg_cur)
{
#if 0
  xmlNodePtr cur = arg_cur->children;

  while (cur != NULL)
    {
      if (strcmp((char*)cur->name, "spring") == 0)
	{
	  xmlNodePtr ccur = cur->children;
	  int first = -1;
	  int second = -1;
	  while (ccur != NULL)
	    {
	      if (strcmp((char*)ccur->name, "first") == 0)
		{	
		  first = to_int(doc, ccur->children);
		}
	      else if (strcmp((char*)ccur->name, "second") == 0)
		{	  
		  second = to_int (doc, ccur->children);
		}
	      ccur = ccur->next;
	    }
	  std::cout << "Spring: " << first << ", " << second << std::endl;
	  sticks.push_back (new Stick (id_to_particle (first), id_to_particle (second)));
	}
      
      cur = cur->next;
    }
#endif
}

void 
ConstruoMain::zero_out_velocity ()
{
  std::cout << "Setting velocity to zero" << std::endl;
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      (*i)->velocity = CL_Vector ();
    }
}

void
ConstruoMain::load_xml (std::string filename)
{
#if 0
  running = false;
  particles.clear ();
  sticks.clear ();

  std::cout << "Quick load from: " << filename << std::endl;  
  xmlDocPtr doc = xmlParseFile(filename.c_str());

  if (doc == NULL) {
    std::cout << "Couldn't open: " << filename << std::endl;
    return;
  }

  std::cout << "Opening successfull: " << doc << std::endl;
  xmlNodePtr cur = xmlDocGetRootElement(doc); //doc->children;

  if (cur == NULL)
    {
      std::cout << "Empty document" << std::endl;
      xmlFreeDoc (doc);
      return; 
    }
  
  std::cout << "Entering parse loop: " << cur << std::endl;
  
  if (cur)
    cur = cur->children;
  
  std::cout << "Children: " << cur << std::endl;

  while (cur != NULL)
    {
      if (xmlIsBlankNode(cur)) {
	cur = cur->next;
	continue;
      }

      std::cout << "Element: " << cur->name << std::endl;

      if (strcmp((char*) cur->name, "particle-list") == 0)
	{
	  std::cout << "Loading particle list" << std::endl;
	  load_particles (doc, cur);
	}
      else if (strcmp((char*)cur->name, "spring-list") == 0)
	{
	  std::cout << "Loading springs list" << std::endl;
	  load_springs (doc, cur);
	}
      else
	{
	  std::cout << "Main loop: Unknown ident: " << (char*)cur->name << std::endl;
	}
      
      cur = cur->next;
    }
  xmlFreeDoc (doc);
#endif
}

void 
ConstruoMain::save_xml (std::string filename)
{
  std::cout << "Quick save to: " << filename << std::endl;
  std::ofstream out (filename.c_str ());

  out << "<?xml version=\"1.0\"?>\n";
  out << "<construo-scene>\n";

  out << "  <particle-list>\n";
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      (*i)->write_xml (out);
    }  
  out << "  </particle-list>\n";

  out << "  <spring-list>\n";
  for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
    {
      (*i)->write_xml (out);
    }  
  out << "  </spring-list>\n";
  
  out << "</construo-scene>\n";
}

void 
ConstruoMain::on_key_release(int key_id)
{
  /*
    if (device == CL_Input::pointers[0])
    {
    Particle* particle = new Particle (CL_Vector (key.x, key.y),
    -3.0 * (CL_Vector (key.x, key.y) - click_pos));
      
    if (last_particle)
    {
    sticks.push_back (new Stick (last_particle, particle));
    last_particle = 0;
    }
    else
    {
    last_particle = particle;
    }

    particles.push_back (particle);
    click_pos = CL_Vector ();
    }*/
}

Particle* 
ConstruoMain::current_particle ()
{
  CL_Vector mouse_pos (input_context->get_mouse_x (), input_context->get_mouse_y ());
  
  Particle* particle = 0;
  float min_dist = 25;
  
  for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
    {
      CL_Vector diff = mouse_pos - (*i)->pos;
      if (diff.norm () < min_dist)
	{
	  min_dist = diff.norm ();
	  particle = *i;
	}
    }
  
  return particle;
}

bool stick_destroyed (Stick* stick)
{
  return stick->destroyed;
}

int 
ConstruoMain::main (int argc, char* argv[])
{
  DeltaManager delta_manager;

  slow_down = false;

  //FIXME:slot_press = CL_Input::sig_button_press ().connect (this, &ConstruoMain::on_press);
  //FIXME:slot_release = CL_Input::sig_button_release ().connect (this, &ConstruoMain::on_release);

  // FIXME: testing stuff
  for (int i = 0; i < 10; ++i)
    {
      Particle* particle = new Particle (CL_Vector (rand()%800, rand()%600), 
                                         CL_Vector ());
      Particle* last_particle = new Particle (CL_Vector (rand()%800, rand()%600), CL_Vector ());
    
      sticks.push_back (new Stick (last_particle, particle));

      particles.push_back (last_particle);
      particles.push_back (particle);
    }

  while (!input_context->get_keycode (KEY_ESCAPE))
    {
      double delta;
      
      if (slow_down)
	delta = delta_manager.getset ()/50.0;
      else
	{
	  if (input_context->get_keycode (KEY_ENTER))
	    delta = delta_manager.getset ();
	  else
	    delta = delta_manager.getset ()/5.0;
	}
      graphic_context->clear ();

      if (1 || running)
	{
	  for (int k = 0;  k < 20; ++k)
	    {
	      {
		for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
		  {
		    // Gravity
		    (*i)->add_force (CL_Vector (0.0, 1.0));

		    
		    // Central Gravity force:
		    /*CL_Vector direction = ((*i)->pos - CL_Vector (400, 300));
                      if (direction.norm () != 0.0f)
		      (*i)->add_force (direction * (-100.0f/(direction.norm () * direction.norm ())));
		    */
		    
		    
		    /*
		      for (ParticleIter j = particles.begin (); j != particles.end (); ++j)
		      {
		      CL_Vector diff = (*j)->pos - (*i)->pos;
		      if (diff.norm () != 0.0f)
		      (*i)->add_force (diff * ((10.0f - (*j)->mass)/(diff.norm () * diff.norm ())));
		      }	    */
		  }


		for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
		  {
		    (*i)->update (delta);
		  }

		for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
		  {

		    (*i)->update (delta);
		    (*i)->clear_force ();

		  }
	      }
	    }
          // Stick splitting
          for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
            {
              if ((*i)->destroyed)
                {
                  if ((((*i)->particles.first->pos 
                        - (*i)->particles.second->pos)).norm () > 10.0f)
                    {
                      CL_Vector pos = 
                        ((*i)->particles.first->pos + (*i)->particles.second->pos)*0.5f;
                      Particle* p1 = new Particle (pos, CL_Vector ());
                      Particle* p2 = new Particle (pos, CL_Vector ());
                      p1->velocity = (*i)->particles.first->velocity * 0.5f;
                      p2->velocity = (*i)->particles.second->velocity * 0.5f;
                      particles.push_back (p1);
                      particles.push_back (p2);
                      sticks.push_back (new Stick ((*i)->particles.first, p1));
                      sticks.push_back (new Stick ((*i)->particles.second, p2));
                    }
                }
            }

          sticks.remove_if (stick_destroyed);
        }

#if 0
      {
        Particle* p = current_particle ();
        if (p)
          {
            int size = 5;
            CL_Display::fill_rect (int(p->pos.x - size), int(p->pos.y - size),
                                   int(p->pos.x + size), int(p->pos.y + size), 
                                   1.0f, 0.0f, 0.0f);
          }
      }
#endif
      for (ParticleIter i = particles.begin (); i != particles.end (); ++i)
        (*i)->draw ();

      for (StickIter i = sticks.begin (); i != sticks.end (); ++i)
        {
          (*i)->draw ();
        }

      graphic_context->flip ();
      KeepAliveMgr::keep_alive ();
      system_context->sleep (1000);
    }
  return 0;
}

int main (int argc, char** argv)
{
  std::cout << "Construo " << VERSION << std::endl;

  X11Display display (800, 600);
  UnixSystem system;
  
  // Init the display, input systems
  graphic_context = &display;
  input_context   = &display;
  system_context  = &system;
  
  ConstruoMain app;
  return app.main (argc, argv);
}

/* EOF */
