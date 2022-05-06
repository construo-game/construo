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

#include "unix_system.hpp"

#include <algorithm>
#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <pwd.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <xdg.h>

#include "construo.hpp"
#include "construo_error.hpp"
#include "path.hpp"
#include "path_manager.hpp"
#include "string_utils.hpp"

using namespace StringUtils;

UnixSystem::UnixSystem () :
  start_time(0),
  m_construo_rc_path()
{
  // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);
  start_time = tv.tv_sec * 1000 + tv.tv_usec / 1000;

  char* home = getenv("HOME");
  if (!home) {
    std::cout << "UnixSystem: FATAL ERROR: couldn't find env variable $HOME" << std::endl;
    throw ConstruoError ("UnixSystem: Couldn't find $HOME!");
  }

  {
    std::filesystem::path const legacy_path = std::filesystem::path(home) / std::filesystem::path(".construo");
    std::filesystem::path const xdg_path = xdg::config().home() / PACKAGE;

    if (std::filesystem::is_directory(xdg_path)) {
      m_construo_rc_path = xdg_path;
    } else if (std::filesystem::is_directory(legacy_path)) {
      m_construo_rc_path = legacy_path;
    } else {
      m_construo_rc_path = xdg_path;
    }
  }

  // create $HOME directory if not already there
  if (!std::filesystem::is_directory(m_construo_rc_path)) {
    std::filesystem::create_directory(m_construo_rc_path);
  }
}

UnixSystem::~UnixSystem ()
{
}

unsigned long
UnixSystem::get_time ()
{ // riped out of ClanLib-0.7
  timeval tv;
  gettimeofday(&tv, NULL);

  unsigned long tid = tv.tv_sec * 1000 + tv.tv_usec / 1000 - start_time;

  return tid;
}

void
UnixSystem::sleep(unsigned int msec)
{
  usleep(msec);
}

std::filesystem::path
UnixSystem::get_construo_rc_path()
{
  return m_construo_rc_path;
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

unsigned long
UnixSystem::get_mtime(const std::string& filename)
{
  std::string sys_name = translate_filename(filename);

  struct stat buf;
  if (stat(sys_name.c_str(), &buf) != 0)
  {
    return 0;
  }
  else
  {
    return static_cast<unsigned long>(buf.st_mtime);
  }
}

FileType
UnixSystem::get_file_type(const std::string& filename)
{
  if (filename == "/examples" ||
      filename == "/user") {
    return FT_DIRECTORY;
  }

  std::filesystem::path sys_name = translate_filename(filename);

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

std::filesystem::path
UnixSystem::translate_filename(const std::string& filename)
{
  if (filename == "/")
  {
    assert(false && "root directory is not translatable");
    return "";
  }
  else if (filename == "/user")
  {
    return m_construo_rc_path;
  }
  else if (filename == "/examples")
  {
    return path_manager.complete("examples");
  }
  else if (filename == "/examples")
  {
    return m_construo_rc_path;
  }
  else if (has_prefix(filename, "/user/"))
  {
    return m_construo_rc_path / filename.substr(6);
  }
  else if (has_prefix(filename, "/examples/"))
  {
    return path_manager.complete("examples") / filename.substr(10);
  }
  else
  {
    return filename;
  }
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

  DirectorySorter(const std::string& p) :
    pathname(p)
  {
  }

  bool operator()(const std::string& lhs, const std::string& rhs)
  {
    FileType const lhs_type = g_system_context->get_file_type(path_join(pathname, lhs));
    FileType const rhs_type = g_system_context->get_file_type(path_join(pathname, rhs));

    if (lhs_type == rhs_type) {
      return (lhs < rhs);
    } else if (lhs_type == FT_DIRECTORY) {
      return true;
    } else if (rhs_type == FT_DIRECTORY) {
      return false;
    } else {
      return (lhs < rhs);
    }
  }
};

std::vector<std::string>
UnixSystem::read_directory(const std::string& arg_pathname)
{
  if (arg_pathname == "/")
  {
    return {"examples", "user" };
  }
  else
  {
    std::vector<std::string> dir_lst;
    std::filesystem::path pathname = translate_filename(arg_pathname);

    for (std::filesystem::directory_entry const& entry : std::filesystem::directory_iterator{pathname})
    {
      std::cout << entry.path().filename() << std::endl;
      if (entry.path().filename() != "." &&
          entry.path().filename() != "..")
      { // We ignore unusefull directories
        dir_lst.push_back(entry.path().filename());
      }
    }

    std::sort(dir_lst.begin(), dir_lst.end(), DirectorySorter(pathname));

    return dir_lst;
  }
}

/* EOF */
