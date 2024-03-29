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

#include "construo_main.hpp"

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
#include "screen_manager.hpp"

namespace construo {

ConstruoMain::ConstruoMain () :
  m_display(),
  m_system(),
  m_do_quit(false)
{
}

ConstruoMain::~ConstruoMain ()
{
}

void
ConstruoMain::init_system()
{
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

    if (!g_settings.datadir.empty()) {
      path_manager.add_path(g_settings.datadir);
    }
    path_manager.add_path(".");
    path_manager.add_path("..");
    path_manager.add_path(CONSTRUO_DATADIR);

    if (!path_manager.find_path("examples"))
    {
      std::cerr << "Couldn't find Construo Datadir, use '--datadir DIR' to set it manually." << std::endl;
      ::exit(EXIT_FAILURE);
    }

    std::unique_ptr<Controller> controller = std::make_unique<Controller>();
    if (!g_settings.startup_file.empty())
    {
      controller->load_world(g_settings.startup_file);
    }
    else
    {
      if (std::filesystem::exists(g_system_context->translate_filename("/user/laststate.construo"))) {
        controller->load_world("/user/laststate.construo");
      }
    }

    // FIXME: get proper x/y position of the window
    ScreenManager::instance()->set_geometry(m_display->geometry());

    // For some targets this will never return
    m_display->run();

    // Shutdown the system and exit
    controller->save_world("/user/laststate.construo");

    std::cout << "\n\n            Thank you for playing Construo!\n\n\n"
              << "  New versions and more information can be found at:\n\n"
              << "    * http://fs.fsf.org/construo/\n\n"
              << "  Comments, critique, suggestions self build\n  construction and patches can be send to:\n\n"
              << "    * Ingo Ruhnke <grumbel@gmail.com>\n\n" << std::endl;

    deinit_system();
  } catch (std::exception const& err) {
    print_exception(err);
    return EXIT_FAILURE;
  }

  return 0;
}

} // namespace construo

/* EOF */
