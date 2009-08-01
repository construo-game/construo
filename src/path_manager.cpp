//  Pingus - A free Lemmings clone
//  Copyright (C) 2000 Ingo Ruhnke <grumbel@gmx.de>
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

#include <iostream>
#include <unistd.h>
#include "path_manager.hpp"

PathManager path_manager;

PathManager::PathManager ()
  : path_found (false)
{
}

PathManager::~PathManager ()
{
}

void
PathManager::add_path (const std::string& path)
{
  path_list.push_back (path);
}

std::string
PathManager::complete (const std::string& relative_path)
{
  std::string comp_path = base_path + "/" + relative_path;

  return comp_path;
}

bool
PathManager::find_path (const std::list<std::string>& file_list)
{
  for (PathIter i = path_list.begin (); !path_found && i != path_list.end (); ++i)
    {
      bool found_file = true;
      for (PathIter f = file_list.begin (); found_file && f != file_list.end (); ++f)
	{
          if (!(access((*i + "/" + *f).c_str(), R_OK) == 0))
              found_file = false;
	}
      if (found_file)
	{
	  path_found = true;
	  base_path = *i;

	  std::cout << "PathManager: Using base_path: " << base_path << std::endl;

	  return true;
	}
    }

  std::cout << "PathManager: No base path found" << std::endl;

  return false;
}

/** Search for a path which contains the file 'file' */
bool
PathManager::find_path (const std::string& file)
{
  for (PathIter i = path_list.begin (); !path_found && i != path_list.end (); ++i)
    {
      if ((access((*i + "/" + file).c_str(), R_OK) == 0))
	{
	  path_found = true;
	  base_path = *i;

	  std::cout << "PathManager: Using base_path: " << base_path << std::endl;

	  return true;
	}
    }

  std::cout << "PathManager: No base path found" << std::endl;

  return false;
}

void
PathManager::set_path (const std::string& path)
{
  base_path = path;
}

/* EOF */
