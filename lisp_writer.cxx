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

#include "lisp_writer.hxx"

LispWriter::LispWriter (const char* name)
{
  lisp_objs.push_back(lisp_make_symbol (name));
}

void
LispWriter::append (lisp_object_t* obj)
{
  lisp_objs.push_back(obj);
}

lisp_object_t*
LispWriter::make_list3 (lisp_object_t* a, lisp_object_t* b, lisp_object_t* c)
{
  return lisp_make_cons (a, lisp_make_cons(b, lisp_make_cons(c, lisp_nil())));
}

lisp_object_t*
LispWriter::make_list2 (lisp_object_t* a, lisp_object_t* b)
{
  return lisp_make_cons (a, lisp_make_cons(b, lisp_nil()));
}

void
LispWriter::write_vector (const char* name, const CL_Vector& pos)
{
  append(lisp_make_cons (lisp_make_symbol (name),
                         make_list3(lisp_make_real(pos.x),
                                    lisp_make_real(pos.y),
                                    lisp_make_real(pos.z))));
}

void
LispWriter::write_float (const char* name, float f)
{
  append(make_list2 (lisp_make_symbol (name),
                     lisp_make_real(f)));
}

void 
LispWriter::write_int (const char* name, int i)
{
  append(make_list2 (lisp_make_symbol (name),
                     lisp_make_integer(i)));
}

void
LispWriter::write_string (const char* name, const char* str)
{
  append(make_list2 (lisp_make_symbol (name),
                     lisp_make_string(str)));
}

void
LispWriter::write_symbol (const char* name, const char* symname)
{
  append(make_list2 (lisp_make_symbol (name),
                     lisp_make_symbol(symname)));
}

void
LispWriter::write_lisp_obj(const char* name, lisp_object_t* lst)
{
  append(make_list2 (lisp_make_symbol (name),
                     lst));
}

void
LispWriter::write_boolean (const char* name, bool b)
{
  append(make_list2 (lisp_make_symbol (name),
                     lisp_make_boolean(b)));
}

lisp_object_t*
LispWriter::get_lisp ()
{
  lisp_object_t* lisp_obj = lisp_nil();
  
  for(std::vector<lisp_object_t*>::reverse_iterator i = lisp_objs.rbegin (); 
      i != lisp_objs.rend (); ++i)
    {
      lisp_obj = lisp_make_cons (*i, lisp_obj);
    }
  lisp_objs.clear();
  
  return lisp_obj;
}

/* EOF */
