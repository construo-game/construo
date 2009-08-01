//  Construo - A wire-frame construction gamee
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

#include "string_utils.hpp"

namespace StringUtils
{
  bool has_prefix (const std::string& str, const std::string& prefix)
  {
    if (prefix.length() > str.length())
      {
        return false;
      }
    else
      {
        for (std::string::size_type i = 0; i < prefix.length(); ++i)
          {
            if (prefix[i] != str[i])
              return false;
          }
        return true;
      }
  }

  bool has_suffix (const std::string& str, const std::string& suffix)
  {
    if (suffix.length() > str.length())
      {
        return false;
      }
    else
      {
        for (std::string::size_type i = 0; i < suffix.length(); ++i)
          {
            if (suffix[i] != str[str.length() -  suffix.length() + i])
              return false;
          }
        return true;
      }
  }
}

/* EOF */
