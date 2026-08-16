// Construo - A wire-frame construction game
// Copyright (C) 2002-2022 Ingo Ruhnke <grumbel@gmail.com>
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

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <typeinfo>

#include <SDL.h>

namespace {

void construo_terminate_handler()
{
  // ArkOS + GCC15 static libgcc_eh / cxxabi shim can fail to unwind; still try
  // to surface the active exception so PortMaster logs show a real message.
  std::fprintf(stderr, "construo: std::terminate() called\n");
  if (std::exception_ptr ep = std::current_exception()) {
    try {
      std::rethrow_exception(ep);
    } catch (std::exception const& e) {
      std::fprintf(stderr, "construo: uncaught exception: %s\n", e.what());
    } catch (...) {
      std::fprintf(stderr, "construo: uncaught non-std exception\n");
    }
  } else {
    std::fprintf(stderr, "construo: terminate without active exception\n");
  }
  std::fflush(stderr);
  std::_Exit(EXIT_FAILURE);
}

} // namespace

// On Android, SDL.h maps main → SDL_main so SDLActivity can dlsym it from
// libmain.so. App logic lives in ConstruoMain::run (not named main) so the
// macro cannot rewrite the method.
int main(int argc, char** argv)
{
  std::set_terminate(construo_terminate_handler);
  construo::ConstruoMain app;
  return app.run(argc, argv);
}

/* EOF */
