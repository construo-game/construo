//  $Id$
// 
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

#ifndef HEADER_CONSTRUO_LISP_WRITER_HPP
#define HEADER_CONSTRUO_LISP_WRITER_HPP

#include <vector>

#include "lispreader.hpp"
#include "vector2d.hpp"

/** */
class LispWriter
{
private:
  std::vector<lisp_object_t*> lisp_objs;
  
  void append (lisp_object_t* obj);
  lisp_object_t* make_list3 (lisp_object_t*, lisp_object_t*, lisp_object_t*);
  lisp_object_t* make_list2 (lisp_object_t*, lisp_object_t*);
public:
  LispWriter (const char* name);
  void write_vector (const char* name, const Vector2d& pos);
  void write_float (const char* name, float f);
  void write_int (const char* name, int i);
  void write_boolean (const char* name, bool b);
  void write_string (const char* name, const char* str);
  void write_symbol (const char* name, const char* symname);
  void write_lisp_obj(const char* name, lisp_object_t* lst);
  
  /** caller is responible to free the returned lisp_object_t */
  lisp_object_t* create_lisp ();
};

#endif

/* EOF */
