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

#ifndef HEADER_CONSTRUO_CONSTRUO_HXX
#define HEADER_CONSTRUO_CONSTRUO_HXX

class SystemContext;
class InputContext;
class GraphicContext;

/** Global accessor to the system functions */
extern SystemContext*  system_context;

/** Global accessor to the input functions */
extern InputContext*   input_context;

/** Global accessor to the graphic device */
extern GraphicContext* graphic_context;

const double back_force = 500.0;
const double max_stretch = 0.1;

#include <string>
#include <iostream>
#include "vector.hxx"

std::string to_xml (const CL_Vector& vec);

// FIXME:
typedef void* xmlDocPtr;
typedef void* xmlNodePtr;

inline
float to_float(xmlDocPtr doc, xmlNodePtr cur)
{
#if 0
  assert (cur && doc);
  if (cur)
    {
      char* ident = (char*)xmlNodeListGetString(doc, cur, 1);
      float i;
      if (sscanf(ident, "%f", &i) != 1)
	std::cout << "Not a float" << std::endl;
      free (ident);
      return i;
    }
  else
    {
      std::cout << "Node empty" << std::endl;
      return 0;
    }
#endif
  return 0.0f;
}

inline
int to_int(xmlDocPtr doc, xmlNodePtr cur)
{
#if 0
  assert (cur && doc);
  if (cur)
    {
      char* ident = (char*)xmlNodeListGetString(doc, cur, 1);
      int i;
      if (sscanf(ident, "%d", &i) != 1)
	std::cout << "Not a int" << std::endl;
      std::cout << "int: " << i << std::endl;
      free (ident);
      return i;
    }
  else
    {
      std::cout << "Node empty" << std::endl;
      return 0;
    }
#endif 
  return 0;
}

inline
CL_Vector cl_vector_from_xml (xmlDocPtr doc, xmlNodePtr cur)
{
#if 0
  std::cout << "parsing vector" << std::endl;
  CL_Vector pos;

  assert (cur);

  if (xmlIsBlankNode(cur)) {
    cur = cur->next;
  }
  cur = cur->children;
  while (cur)
    {
      if (xmlIsBlankNode(cur)) {
	cur = cur->next;
	continue;
      }

      if (strcmp((char*)cur->name, "x") == 0) {
	pos.x = to_float(doc, cur->children);
      } else if (strcmp((char*)cur->name, "y") == 0) {
	pos.y = to_float(doc, cur->children);
      } else if (strcmp((char*)cur->name, "z") == 0) {
	pos.z = to_float(doc, cur->children);
      } else {
	std::cout << "cl_vector_from_xml: Unknown ident: " << (char*)cur->name << std::endl;
      }
      cur = cur->next;
    }
  return pos;
#endif
  return CL_Vector ();
}

#endif

/* EOF */
