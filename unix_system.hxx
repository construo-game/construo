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

#ifndef HEADER_CONSTRUO_UNIX_SYSTEM_HXX
#define HEADER_CONSTRUO_UNIX_SYSTEM_HXX

#include <string>
#include "system_context.hxx"

/** This class wraps a bunch of unix functions to provide file-io and
    the current time */
class UnixSystem : public SystemContext
{
private:
  unsigned int start_time;
  std::string construo_rc_path;
  
  std::string translate_filename (const std::string&);
public:
  UnixSystem ();
  virtual ~UnixSystem ();
  unsigned int get_time ();
  void sleep (unsigned long);
  std::string get_construo_rc_path();
  std::string get_user_realname();
  std::string get_user_email();

  FILE* open_input_file(const std::string& filename);
  FILE* open_output_file(const std::string& filename);

  unsigned int get_mtime (const std::string& filename);

  FileType get_file_type(const std::string& filename);
  std::vector<std::string> read_directory(const std::string& pathname);
};

#endif

/* EOF */
