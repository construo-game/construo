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

#ifndef HEADER_ZOOM_GRAPHIC_CONTEXT_HXX
#define HEADER_ZOOM_GRAPHIC_CONTEXT_HXX

#include "graphic_context.hxx"

/** A GraphicContext, that wraps another GraphicContext and adds
    coordinate translation like zoom and scrolling */
class ZoomGraphicContext : public GraphicContext
{
private:
  GraphicContext* parent_gc;

  float x_offset;
  float y_offset;
  float zoom;
  int   zoom_stage;

  int translate_x (int);
  int translate_y (int);
public:
  ZoomGraphicContext ();

  void draw_line(int x1, int y1, int x2, int y2, Color color, int wide = 0);
  void draw_rect(int x1, int y1, int x2, int y2, Color color);
  void draw_circle(int x, int y, int r, Color color);
  void draw_fill_circle(int x, int y, int r, Color color);
  void draw_fill_rect(int x1, int y1, int x2, int y2, Color color);
  void draw_string(int x, int y, const std::string& str, Color color = Color (0xFFFFFF));

  int get_width ()  { return parent_gc->get_width (); }
  int get_height () { return parent_gc->get_height (); }

  void clear ()  { parent_gc->clear (); }

  /** FIXME: flip should be handled outsite of GraphicContext */
  void flip () { parent_gc->flip (); }

  void set_parent_gc (GraphicContext* gc);

  bool zoom_in (int x, int y);
  bool zoom_out (int x, int y);
  void zoom_to (int x1, int y1, int x2, int y2);
  void translate_offset (int x, int y);
  void set_offset (float x, float y);

  float get_x_offset () { return x_offset; }
  float get_y_offset () { return y_offset; }

  float get_zoom () { return zoom; }

  int screen_to_world_x (int);
  int screen_to_world_y (int);

  int world_to_screen_x (int);
  int world_to_screen_y (int);

  void flip (int x1, int y1, int x2, int y2);
};

#endif

/* EOF */
