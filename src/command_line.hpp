// Construo - A wire-frame construction gamee
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

#ifndef HEADER_CONSTRUO_COMMAND_LINE_HPP
#define HEADER_CONSTRUO_COMMAND_LINE_HPP

#include <ostream>
#include <string>

/** Parses the command line */
class CommandLine
{
public:
  static void parse(int argc, char** argv);
  static void print_help(std::ostream& out);
  static void print_version();

private:
  static void error(const std::string& msg);
};

#endif

/* EOF */
