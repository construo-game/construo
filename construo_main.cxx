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
#include "world.hxx"
#include "construo_error.hxx"

#include "x11_display.hxx"
#include "unix_system.hxx"
#include "particle_factory.hxx"

ConstruoMain* construo_main;

ConstruoMain::ConstruoMain ()
{
  last_particle = 0;
  running = false;
  c_particle = 0;
  do_quit = false;
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
ConstruoMain::on_exit()
{
  if (!world->get_has_been_run())
    save_world(system_context->get_construo_rc_path() + "laststate.construo");
}

void 
ConstruoMain::zero_out_velocity ()
{
  std::cout << "Setting velocity to zero" << std::endl;
  for (ParticleFactory::ParticleIter i = world->get_particle_mgr()->begin(); 
       i != world->get_particle_mgr()->end (); ++i)
    {
      (*i)->velocity = CL_Vector ();
    }
}

void
ConstruoMain::process_events ()
{
  Event event;
  int x = input_context->get_mouse_x();
  int y = input_context->get_mouse_y();

  while (input_context->get_event (&event))
    {
      if (event.button.pressed)
        {
          switch (event.type)
            {
            case BUTTON_EVENT:
              switch (event.button.id)
                {
                case BUTTON_START:
                  {
                    if (!running)
                      undo_world_stack.push_back(world->duplicate());
                    running = !running;
                  }
                  break;
                case BUTTON_PRIMARY:
                  if (c_particle)
                    {
                      Particle* nc_particle = world->get_particle (x, y);
                      if (nc_particle != c_particle)
                        {
                          if (nc_particle) // connect to particles
                            {
                              world->add_spring (c_particle, nc_particle);
                            }
                          else // add a new particle and connect it with the current one
                            {
                              nc_particle = world->get_particle_mgr()->add_particle (CL_Vector(x, y), CL_Vector());
                              world->add_spring (c_particle, nc_particle);
                            }
                          c_particle = 0;
                        }
                    }
                  else
                    {
                      c_particle = world->get_particle (input_context->get_mouse_x(),
                                                        input_context->get_mouse_y());
                      if (!c_particle)
                        {
                          Particle* p = world->get_particle_mgr()->add_particle (CL_Vector(x, y), CL_Vector());
                          c_particle = p;
                        }
                    }
                  break;
                case BUTTON_FIX:
                  {
                    Particle* particle = world->get_particle (x, y);
                    std::cout << "Fixing particle: " << particle << std::endl;
                    if (particle)
                      {
                        std::cout << "particle: " << particle->get_fixed () << std::endl;
                        particle->set_fixed (!particle->get_fixed ());
                      }
                  }
                  break;
                case BUTTON_DELETE:
                  std::cout << "Deleteing " << c_particle << std::endl;
                  if (c_particle)
                    {
                      c_particle = 0;
                    }
                  else
                    {
                      Particle* p = world->get_particle (input_context->get_mouse_x(),
                                                         input_context->get_mouse_y());
                      world->remove_particle (p);
                    }
                  break;
                case BUTTON_CLEAR:
                  std::cout << "Clear" << std::endl;
                  undo_world_stack.push_back(world);
                  world = new World ();
                  c_particle = 0;
                  running = false;
                  break;
                  
                case BUTTON_UNDO:
                  if (!undo_world_stack.empty())
                    {
                      //delete world; // fixme: memory hole
                      redo_world_stack.push_back (world);
                      world = undo_world_stack.back();
                      undo_world_stack.pop_back();
                      running = false;
                    }
                  else
                    {
                      std::cout << "Undo stack empty" << std::endl;
                    }
                  break;
                  
                case BUTTON_REDO:
                  if (!redo_world_stack.empty())
                    {
                      undo_world_stack.push_back (world);
                      world = redo_world_stack.back();
                      redo_world_stack.pop_back();
                      running = false;
                    }
                  else
                    {
                      std::cout << "Redo stack empty" << std::endl;
                    }
                  break;

                case BUTTON_ESCAPE:
                  do_quit = true;
                  break;

                case BUTTON_TOGGLESLOWMO:
                  slow_down = !slow_down;
                  break;

                case BUTTON_QUICKSAVE0:
                case BUTTON_QUICKSAVE1:
                case BUTTON_QUICKSAVE2:
                case BUTTON_QUICKSAVE3:
                case BUTTON_QUICKSAVE4:
                case BUTTON_QUICKSAVE5:
                case BUTTON_QUICKSAVE6:
                case BUTTON_QUICKSAVE7:
                case BUTTON_QUICKSAVE8:
                case BUTTON_QUICKSAVE9:
                  save_to_slot (event.button.id - BUTTON_QUICKSAVE0);
                  break;

                case BUTTON_QUICKLOAD0:
                case BUTTON_QUICKLOAD1:
                case BUTTON_QUICKLOAD2:
                case BUTTON_QUICKLOAD3:
                case BUTTON_QUICKLOAD4:
                case BUTTON_QUICKLOAD5:
                case BUTTON_QUICKLOAD6:
                case BUTTON_QUICKLOAD7:
                case BUTTON_QUICKLOAD8:
                case BUTTON_QUICKLOAD9:
                  load_from_slot (event.button.id - BUTTON_QUICKLOAD0);
                  break;

                default:
                  std::cout << "Got unhandled BUTTON_EVENT: " << event.button.id << std::endl;
                  break;
                }
              //std::cout << "GOt Event: " << event.button.id << std::endl;
              break;
            default:
              std::cout << "ConstruoMain: Unhandled event: " << event.type << std::endl;
            }
        }
    }
}

void
ConstruoMain::load_world (const std::string& filename)
{
  undo_world_stack.push_back(world);
  std::cout << "Loading World..." << std::endl;
  world = new World (filename);
  running = false;
  std::cout << "Loading World... DONE" << std::endl; 
}

void
ConstruoMain::save_world (const std::string& filename)
{
  std::cout << "Saving World..." << std::endl;
  world->write_lisp (filename);
  std::cout << "Saving World... DONE" << std::endl;
}

int 
ConstruoMain::main (int argc, char* argv[])
{
  DeltaManager delta_manager;

  slow_down = false;

  //FIXME:slot_press = CL_Input::sig_button_press ().connect (this, &ConstruoMain::on_press);
  //FIXME:slot_release = CL_Input::sig_button_release ().connect (this, &ConstruoMain::on_release);

  if (argc == 2)
    {
      world = new World (argv[1]);
    }
  else
    {
      try {
        world = new World(system_context->get_construo_rc_path() + "laststate.construo");
      } catch (ConstruoError& err) {
        std::cout << "ConstruoMain: " << err.msg << std::endl;
        world = new World ();
      }
    }

  while (!do_quit)
    {
      process_events ();

      double delta = delta_manager.getset ();

      if (running)
        {
          float min_skip;

          if (slow_down)
            {
              delta /= 50.0f;
              min_skip = 0.0007f;
            }
          else
            {
              delta /= 5.0f;
              min_skip = 0.007f;
            }
      
          float i = 0.0f;
          while (i < delta)
            {
              world->update (min_skip);
              i += min_skip;
            }
        }

      graphic_context->clear ();
      world->draw (graphic_context);
          
      { // draw 'marker'
        Particle* p = world->get_particle (input_context->get_mouse_x (),
                                           input_context->get_mouse_y ());
        if (p)
          {
            p->draw_highlight (graphic_context);
          }

        if (c_particle)
          {
            if (p)
              graphic_context->draw_line (int(c_particle->pos.x), int(c_particle->pos.y),
                                          int(p->pos.x), int(p->pos.y),
                                          Color(0.3f, 0.3f, 0.3f));
            else
              graphic_context->draw_line (int(c_particle->pos.x), int(c_particle->pos.y),
                                          input_context->get_mouse_x(), input_context->get_mouse_y(),
                                          Color(0.3f, 0.3f, 0.3f));
          }
      }

      graphic_context->draw_string (10, 20, "..:: Construo V"VERSION" ::..");
      graphic_context->draw_string (10, 32, "=========================");
   
      graphic_context->draw_string (400, 20, "      [1-9] - quick save");
      graphic_context->draw_string (400, 32, "[shift 1-9] - quick load");
      graphic_context->draw_string (400, 44, "   [escape] - quit");
      graphic_context->draw_string (400, 56, "    [space] - toggle slow motion");

      graphic_context->draw_string (600,  20, "  [left] - insert/connect spots");
      graphic_context->draw_string (600,  32, "[middle] - start/stop simulation");
      graphic_context->draw_string (600,  44, " [right] - remove spot");
      graphic_context->draw_string (600,  56, "     [c] - clear screen");
      graphic_context->draw_string (600,  68, "     [f] - fix current spot");
      graphic_context->draw_string (600,  80, "     [u] - undo to last state");
      graphic_context->draw_string (600,  92, "     [r] - redo (undo an undo)");

      if (running)
        graphic_context->draw_string (graphic_context->get_width () - 60,
                                      graphic_context->get_height () - 10,
                                      "[RUNNING]", Color(0xFF0000));
      else
        graphic_context->draw_string (graphic_context->get_width () - 60,
                                      graphic_context->get_height () - 10,
                                      "[STOPPED]", Color(0x00FF00));

      if (slow_down)
        {
          graphic_context->draw_string (10,
                                        graphic_context->get_height () - 10,
                                        "[SLOW-MOTION]", Color(0x00FFFF));
        }

      graphic_context->flip ();
      KeepAliveMgr::keep_alive ();
      system_context->sleep (1000);
    }

  on_exit();

  delete world;

  return 0;
}

int main (int argc, char** argv)
{
  if (argc == 2 && argv[1][0] == '-')
    {
      std::cout << "Usage: " << argv[0] << " [FILENAME]" << std::endl;
      return EXIT_FAILURE;
    }

  try {
    X11Display display (800, 600);
    UnixSystem system;
  
    // Init the display, input systems
    graphic_context = &display;
    input_context   = &display;
    system_context  = &system;
  
    std::cout << PACKAGE_STRING"\n" << std::endl;
    std::cout << "If you have throuble with programm startup, delete the file:\n\n" 
              << "    " << system_context->get_construo_rc_path() << "laststate.construo\n" << std::endl;

    ConstruoMain app;
    construo_main = &app;

    return app.main (argc, argv);
  } catch (ConstruoError& err) {
    std::cout << "Error ocurred: " << err.msg << std::endl;
    return EXIT_FAILURE;
  }
}

std::string
ConstruoMain::get_slot_filename(int n)
{
  const std::string& str = system_context->get_construo_rc_path();
  return str + std::string("quicksave") + char('0' + n) + ".construo";
}

void
ConstruoMain::save_to_slot (int n)
{
  try {
    save_world (get_slot_filename (n));
  } catch (ConstruoError& err) {
    std::cout << "ConstruoMain: Error: " << err.msg << std::endl;
  }
}

void
ConstruoMain::load_from_slot (int n)
{
  try {
    load_world (get_slot_filename (n));
  } catch (ConstruoError& err) {
    std::cout << "ConstruoMain: Error: " << err.msg << std::endl;
  }
}

/* EOF */
