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

#ifndef HEADER_CONSTRUO_CONSTRUO_MAIN_HPP
#define HEADER_CONSTRUO_CONSTRUO_MAIN_HPP

#include <memory>
#include <vector>
#include <list>

#include "fwd.hpp"

class ConstruoMain
{
public:
  ConstruoMain ();
  virtual ~ConstruoMain ();

  int main(int argc, char* argv[]);

private:
  /** Initialize the graphic context and the system */
  void init_system();

  /** Shutdown the graphic display and everything else */
  void deinit_system();

  void process_events ();

private:
#ifdef USE_X11_DISPLAY
  std::unique_ptr<X11Display>  m_display;
#elif USE_GLUT_DISPLAY
  std::unique_ptr<GlutDisplay> m_display;
#endif
  std::unique_ptr<UnixSystem> m_system;

  bool m_do_quit;

public:
  ConstruoMain(const ConstruoMain&) = delete;
  ConstruoMain& operator=(const ConstruoMain&) = delete;
};

#endif

/* EOF */
