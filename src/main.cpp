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

// Declare ConstruoMain::main before SDL.h maps main→SDL_main (Android).
#include "construo_main.hpp"

#include <SDL.h>

// On Android, SDL.h maps main → SDL_main so SDLActivity can dlsym it from
// libmain.so. Undef after the entry declarator so app.main() stays the
// ConstruoMain method (declared above, before the macro).
int main(int argc, char** argv)
{
#undef main
  construo::ConstruoMain app;
  return app.main(argc, argv);
}

/* EOF */
