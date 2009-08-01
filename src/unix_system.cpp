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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <errno.h>
#include <iostream>
#include <algorithm>
#include "construo_error.hpp"
#include "string_utils.hpp"
#include "construo.hpp"
#include "path_manager.hpp"
#include "unix_system.hpp"

using namespace StringUtils;

UnixSystem::UnixSystem ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);
  start_time = (long) tv.tv_sec*(long) 1000+(long) tv.tv_usec/(long) 1000;

  char* home = getenv("HOME");
  if (home)
    {
      construo_rc_path = std::string(home) + std::string("/.construo/");
    }
  else
    {
      std::cout << "UnixSystem: FATAL ERROR: couldn't find env variable $HOME" << std::endl;
      throw ConstruoError ("UnixSystem: Couldn't find $HOME!");
    }

  // create $HOME directory if not already there
  struct stat buf;

  if (stat(construo_rc_path.c_str(), &buf) != 0) // Couldn't find directory, create it
    {
      if (mkdir(construo_rc_path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IXGRP) != 0)
        {
          throw ConstruoError(std::string("UnixSystem: ") + construo_rc_path + ": "
                              + strerror(errno));
        }
    }
  else
    {
      if (S_ISDIR(buf.st_rdev)) // Is not a directory
        {
          throw ConstruoError("Error: " + construo_rc_path + " is not a directory!");
        }
      
      if (access(construo_rc_path.c_str (), R_OK | W_OK | X_OK) != 0) // not readable/writeable
        {
          throw ConstruoError("Error: " + construo_rc_path + " is not read or writeable!");
        }
    }     
}

UnixSystem::~UnixSystem ()
{
}

unsigned int 
UnixSystem::get_time ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);

  long tid = (long) tv.tv_sec*(long) 1000 + (long) tv.tv_usec/(long) 1000 - start_time;

  return tid;
}

void
UnixSystem::sleep (unsigned long t)
{
  usleep (t);
}

std::string 
UnixSystem::get_construo_rc_path()
{
  return construo_rc_path;
}

std::string
UnixSystem::get_user_realname()
{
  struct passwd* pw;

  pw = getpwuid(getuid());
  if (pw)
    {
      return pw->pw_gecos;
    }
  else
    {
      return "";
    }
}

std::string 
UnixSystem::get_user_email()
{
  const char* s_email = getenv("EMAIL");
  if (s_email)
    {
      return s_email;
    }
  else
    return "";
}

unsigned int
UnixSystem::get_mtime (const std::string& filename)
{
  std::string sys_name = translate_filename(filename);

  struct stat buf;
  if (stat(sys_name.c_str(), &buf) != 0)
    {
      return 0;
    }
  else
    {
      return buf.st_mtime;
    }
}

FileType
UnixSystem::get_file_type(const std::string& filename)
{
  if (filename == "/examples/"
      || filename == "/user/")
    return FT_DIRECTORY;

  std::string sys_name = translate_filename(filename);
  
  struct stat buf;
  if (stat(sys_name.c_str(), &buf) != 0)
    {
      std::cout << "UnixSystem: ERROR: Couldn't stat: '" << sys_name << "'" << std::endl;
      return FT_UNKNOWN_FILE;
    }
  else
    {
      if (S_ISDIR(buf.st_mode))
        {
          return FT_DIRECTORY;
        }
      else if (S_ISREG(buf.st_mode))
        {
          if (has_suffix(filename, ".construo") || has_suffix(filename, ".construo.gz"))
            return FT_CONSTRUO_FILE;
          else
            {
              return FT_UNKNOWN_FILE;
            }
        }
      else
        {
          return FT_UNKNOWN_FILE;
        }
    }
}

std::string
UnixSystem::translate_filename (const std::string& filename)
{
  if (filename == "/")
    {
      assert("root directory is not translatable");
      return "";
    }
  else if (has_prefix(filename, "/user/"))
    {
      return construo_rc_path + filename.substr(6);
    }
  else if (has_prefix(filename, "/examples/"))
    { 
      return path_manager.complete("examples/") + filename.substr(10); 
    }
  else
    return filename;
}

FILE*
UnixSystem::open_input_file(const std::string& filename)
{
  //std::cout << "UnixSystem: open_input_file: " << translate_filename (filename) << std::endl;
  return fopen(translate_filename (filename).c_str(), "r");
}

FILE*
UnixSystem::open_output_file(const std::string& filename)
{
  //std::cout << "UnixSystem: open_output_file: " << translate_filename (filename) << std::endl;
  return fopen(translate_filename (filename).c_str(), "w");
}


/** Sort directories before files and sort them all
    alphabetically */
struct DirectorySorter
{
  std::string pathname;

  DirectorySorter(const std::string& p)
    : pathname(p)
  {
  }

  bool operator()(const std::string& lhs, const std::string& rhs)
  {
    FileType lhs_type = system_context->get_file_type(pathname + "/" + lhs);
    FileType rhs_type = system_context->get_file_type(pathname + "/" + rhs);
    
    if (lhs_type == rhs_type)
      return (lhs < rhs);
    else if (lhs_type == FT_DIRECTORY)
      {
        return true;
      }
    else if (rhs_type == FT_DIRECTORY)
      {
        return false;
      }
    else 
      {
        return (lhs < rhs);
      }
  }
};

std::vector<std::string>
UnixSystem::read_directory(const std::string& arg_pathname)
{
  if (arg_pathname == "/")
    {
      std::vector<std::string> ret;
      ret.push_back("examples/");
      ret.push_back("user/");
      return ret;
    }
  else
    {
      std::vector<std::string> dir_lst;
      std::string pathname = translate_filename (arg_pathname);

      DIR* dir = ::opendir (pathname.c_str());
      
      if (!dir)
        {
          std::cout << "UnixSystem: Error couldn't open: '" << pathname << "', ignoring\n"
                    << "            error and continuing with an empty directory" << std::endl;
        }
      else
        {
          struct dirent* entry;

          while ((entry = readdir(dir)) != 0)
            {
              if (strcmp(entry->d_name, ".") != 0
                  && strcmp(entry->d_name, "..") != 0
                  && strcmp(entry->d_name, "CVS") != 0)
                { // We ignore unusefull directories
                  dir_lst.push_back(entry->d_name);
                }
            }
  
          closedir (dir);
        }

      std::sort(dir_lst.begin(), dir_lst.end(), DirectorySorter(pathname));

      return dir_lst;
    }
}

/* EOF */
