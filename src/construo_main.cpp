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

#include <fstream>
#include "construo.hpp"
#include "particle.hpp"
#include "world.hpp"
#include "path_manager.hpp"
#include "construo_error.hpp"

#if defined(USE_X11_DISPLAY)
#  include "x11_display.hpp"
#  include "unix_system.hpp"
#elif defined(USE_GLUT_DISPLAY)
#  include "glut_display.hpp"
#  include "unix_system.hpp"
#else
#  error "No Display target defined!"
#endif

#include "controller.hpp"
#include "command_line.hpp"
#include "settings.hpp"
#include "gui_manager.hpp"

#include "construo_main.hpp"

Controller*   controller;

ConstruoMain::ConstruoMain () :
  m_display(),
  m_system(),
  m_do_quit(false),
  m_gui_manager()
{
}

ConstruoMain::~ConstruoMain ()
{
}

void
ConstruoMain::on_exit()
{
  //std::cout << "Calling on_exit()" << std::endl;

  //if (!controller->has_been_run())
  {
    controller->save_world("/user/laststate.construo");
  }

  std::cout << "\n\n            Thank you for playing Construo!\n\n\n"
            << "  New versions and more information can be found at:\n\n"
            << "    * http://fs.fsf.org/construo/\n\n"
            << "  Comments, critique, suggestions self build\n  construction and patches can be send to:\n\n"
            << "    * Ingo Ruhnke <grumbel@gmail.com>\n\n" << std::endl;
}

void
ConstruoMain::init_system()
{
  //std::cout << "ConstruoMain::init_system()" << std::endl;

  std::string const title = "Construo " VERSION;

  m_system = std::make_unique<UnixSystem>();
#ifdef USE_X11_DISPLAY
  m_display = std::make_unique<X11Display>(title, g_settings.screen_width, g_settings.screen_height,
                                         g_settings.fullscreen);
#elif USE_GLUT_DISPLAY
  m_display = std::make_unique<GlutDisplay>(title, g_settings.screen_width, g_settings.screen_height, g_settings.fullscreen);
#else
#  error "No display type defined"
#endif

  // Init the display, input systems
  g_graphic_context = m_display.get();
  g_input_context   = m_display.get();
  g_system_context  = m_system.get();
}

void
ConstruoMain::deinit_system()
{
  //std::cout << "ConstruoMain::deinit_system()" << std::endl;
  m_display.reset();
  m_system.reset();
}

int
ConstruoMain::main(int argc, char* argv[]) // FIXME: pass an option class, instead command line arguments
{
  CommandLine::parse(argc, argv);

  try {
    // Init the System
    init_system();

    std::cout << PACKAGE_STRING"\n" << std::endl;
    std::cout << "If you have throuble with programm startup, delete the file:\n\n"
              << "    " << (g_system_context->get_construo_rc_path() / "laststate.construo") << "\n" << std::endl;

    if (!g_settings.datadir.empty())
      path_manager.add_path(g_settings.datadir);

    path_manager.add_path(".");
    path_manager.add_path("..");
    path_manager.add_path(CONSTRUO_DATADIR);
    if (!path_manager.find_path("examples"))
      {
        std::cout << "Couldn't find Construo Datadir, use '--datadir DIR' to set it manually." << std::endl;
        ::exit(EXIT_FAILURE);
      }

    m_gui_manager = std::make_unique<GUIManager>();

    if (!g_settings.startup_file.empty())
      {
        controller = new Controller (g_settings.startup_file);
      }
    else
      {
        try
          {
            controller = new Controller ("/user/laststate.construo");
          }
        catch (std::exception const& err)
          {
            print_exception(err);
            controller = new Controller ();
          }
      }

    // For some targets this will never return
    m_display->run();

    // Shutdown the system and exit
    on_exit();
    m_gui_manager.reset();
    deinit_system();
  } catch (std::exception const& err) {
    print_exception(err);
    return EXIT_FAILURE;
  }

  return 0;
}

int main (int argc, char** argv)
{
  ConstruoMain app;
  return app.main(argc, argv);
}

/* EOF */
