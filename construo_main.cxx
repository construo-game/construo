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
#include "construo.hxx"
#include "particle.hxx"
#include "config.h"
#include "world.hxx"
#include "construo_error.hxx"

#ifdef USE_X11_DISPLAY
#  include "x11_display.hxx"
#  include "unix_system.hxx"
#else 
#  ifdef USE_GLUT_DISPLAY
#    include "glut_display.hxx"
#    include "unix_system.hxx"
#  endif
#endif

#include "controller.hxx"
#include "command_line.hxx"
#include "settings.hxx"
#include "gui_manager.hxx"

#include "construo_main.hxx"

ConstruoMain* construo_main;
Controller*   controller;

ConstruoMain::ConstruoMain ()
{
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
  std::cout << "Calling on_exit()" << std::endl;

  //if (!controller->has_been_run())
    {
      controller->save_world("/user/laststate.construo");
    }

  std::cout << "\n\n            Thank you for playing Construo!\n\n\n"
            << "  New versions and more information can be found at:\n\n"
            << "    * http://fs.fsf.org/construo/\n\n"
            << "  Comments, critique, suggestions self build\n  construction and patches can be send to:\n\n"
            << "    * Ingo Ruhnke <grumbel@gmx.de>\n\n" << std::endl;
}

int 
ConstruoMain::main (int argc, char* argv[]) // FIXME: pass an option class, instead command line arguments
{
  CommandLine::parse(argc, argv);

  try {
#ifdef USE_X11_DISPLAY
    X11Display display (settings.screen_width, settings.screen_height, 
                        settings.fullscreen);
#elif USE_GLUT_DISPLAY
    GlutDisplay display (settings.screen_width, settings.screen_height);
#else
#  error "No display type defined"
#endif
    UnixSystem system;

    // Init the display, input systems
    graphic_context = &display;
    input_context   = &display;
    system_context  = &system;
  
    std::cout << PACKAGE_STRING"\n" << std::endl;
    std::cout << "If you have throuble with programm startup, delete the file:\n\n" 
              << "    " << system_context->get_construo_rc_path() << "laststate.construo\n" << std::endl;

    GUIManager* gui_manager = new GUIManager ();

    if (!settings.startup_file.empty())
      {
        controller  = new Controller (settings.startup_file);
      }
    else
      {
        try 
          {
            controller = new Controller ("/user/laststate.construo");
          } 
        catch (ConstruoError& err) 
          {
            std::cout << "ConstruoMain: " << err.msg << std::endl;
            controller = new Controller ();
          }
      }
  
    display.run();

    on_exit();
    
    delete gui_manager;

  } catch (ConstruoError& err) {
    std::cout << "Error ocurred: " << err.msg << std::endl;
    return EXIT_FAILURE;
  }

  return 0;
}

////////////////////////
// Real Main Function //
////////////////////////
int main (int argc, char** argv)
{
  ConstruoMain app;
  construo_main = &app;
  return app.main (argc, argv);
}

/* EOF */
