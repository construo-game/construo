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

#ifndef HEADER_CONSTRUO_STRING_UTILS_HPP
#define HEADER_CONSTRUO_STRING_UTILS_HPP

#include <string>
#include <sstream>

namespace StringUtils
{
  template <class T>
  std::string to_string (const T& any)
  {
    std::ostringstream oss;
    oss << any;
    return oss.str();
  }

  /** Convert the contents in string \a rep to type \a T, if conversion
      fails false is returned and the value of \a x is unchanged, if
      true is returned the conversation was successfull. */
  template <class T>
  bool from_string(const std::string& rep, T& x)
  {
    // this is necessary so that if "x" is not modified if the conversion fails
    T temp;
    std::istringstream iss(rep);
    iss >> temp;

    if (iss.fail())
      {
        return false;
      }
    else
      {
        x = temp;
        return true;
      }
  }

  bool has_prefix (const std::string& str, const std::string& prefix);
  bool has_suffix (const std::string& str, const std::string& suffix);
}

#endif

/* EOF */
