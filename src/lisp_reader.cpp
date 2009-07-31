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

#include <string.h>

#include "construo_error.hpp"
#include "lisp_reader.hpp"

LispReader::LispReader (lisp_object_t* l)
  : lst (l)
{
  //std::cout << "LispReader: " << std::flush;
  //lisp_dump(lst, stdout);
  //std::cout << std::endl;
}

lisp_object_t*
LispReader::search_for(const char* name)
{
  //std::cout << "LispReader::search_for(" << name << ")" << std::endl;
  lisp_object_t* cursor = lst;

  while(!lisp_nil_p(cursor))
    {
      lisp_object_t* cur = lisp_car(cursor);

      if (!lisp_cons_p(cur) || !lisp_symbol_p (lisp_car(cur)))
        {
          lisp_dump(cur, stdout);
          throw ConstruoError (std::string("LispReader: Read error in search_for ") + name);
        }
      else
        {
          if (strcmp(lisp_symbol(lisp_car(cur)), name) == 0)
            {
              return lisp_cdr(cur);
            }
        }
      
      cursor = lisp_cdr (cursor);
    }
  return 0;
}

bool
LispReader::read_vector (const char* name, Vector2d* vec)
{
  lisp_object_t* obj = search_for (name);
  if (obj)
    {
      vec->x = lisp_real(lisp_car(obj));
      vec->y = lisp_real(lisp_car(lisp_cdr(obj)));
      return true;
    }
  return false;
}

bool
LispReader::read_int (const char* name, int* i)
{
  lisp_object_t* obj = search_for (name);
  if (obj)
    {
      *i = lisp_integer(lisp_car(obj));
      return true;
    }
  return false;
}

bool
LispReader::read_float (const char* name, float* f)
{
  lisp_object_t* obj = search_for (name);
  if (obj)
    {
      *f = lisp_real(lisp_car(obj));
      return true;
    }
  return false;
}

bool
LispReader::read_bool (const char* name, bool* b)
{
  lisp_object_t* obj = search_for (name);
  if (obj)
    {
      *b = lisp_boolean(lisp_car(obj));
      return true;
    }
  return false;
}

/* EOF */
