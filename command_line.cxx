//  $Id$
//
//  Pingus - A free Lemmings clone
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

#include <iostream>
#include "settings.hxx"
#include "command_line.hxx"

void
CommandLine::error (const std::string& msg)
{
  std::cout << "CommandLine parse failure!" << std::endl;
  std::cout << "CommandLine: " << msg << std::endl;
  exit(EXIT_FAILURE);
}

void
CommandLine::parse (int argc, char** argv)
{
  for (int i = 1; i < argc; ++i) // Skip Programm name
    {
      std::cout << "Arg: " << argv[i] << std::endl;

      if (argv[i][0] == '-') // Argument is an option
        {
          if (strcmp(argv[i], "--fullscreen") == 0
              || strcmp(argv[i], "-f") == 0)
            {
              settings.fullscreen = true;
            }
          else if (strcmp(argv[i], "-g") == 0
                   || strcmp(argv[i], "--geometry") == 0)
            {
              if (++i < argc)
                {
                  char c;
                  if (sscanf(argv[i], "%d%c%d", 
                             &settings.screen_width, &c, &settings.screen_height) != 3 && c != 'x')
                    {
                      error ("geometry string must WIDTHxHEIGHT");
                    }                
                }
              else
                {
                  error ("-g/--geometry is missing an argument");
                }
            }
          else
            {
              error (std::string("Unknown option: ") + argv[i]);
            }
        }
      else
        {
          if (!settings.startup_file.empty())
            {
              error ("Only one filename can be given");
            }
          else
            {
              settings.startup_file = argv[i];
            }
        }
    }
}

/* EOF */
