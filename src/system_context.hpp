//  Construo - A wire-frame construction game
//  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//  
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SYSTEM_CONTEXT_HPP
#define HEADER_SYSTEM_CONTEXT_HPP

#include <stdio.h>
#include <string>
#include <vector>

/** Simple enum for the different file types that Constro supports,
 *  since it doesn't really support much, it only contains directory,
 *  construo file and unknown file */
enum FileType {
  FT_DIRECTORY,
  FT_CONSTRUO_FILE,
  FT_UNKNOWN_FILE
};

/** System stuff like file-IO and time */
class SystemContext
{
private:
public:
  virtual unsigned int get_time () =0;

  /** sleep for the given number of milisec */
  virtual void sleep (unsigned long) =0;

  /** @return '$HOME/.construo/' */
  virtual std::string get_construo_rc_path () =0;

  /** @return the realname of the current user */
  virtual std::string get_user_realname() =0;

  /** @return the email of the current user or an empty string if not available */
  virtual std::string get_user_email() =0;

  /** @return the type of the given file */
  virtual FileType get_file_type(const std::string& filename) =0;

  virtual FILE* open_input_file(const std::string& filename) =0;
  virtual FILE* open_output_file(const std::string& filename) =0;

  virtual unsigned int get_mtime (const std::string& filename) =0;

  /** @return a list of files available in the given directory */
  virtual std::vector<std::string> read_directory(const std::string& pathname) =0;

  virtual std::string translate_filename (const std::string&) =0;
};

#endif

/* EOF */
