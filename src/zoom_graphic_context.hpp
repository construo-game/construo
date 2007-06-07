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

#ifndef HEADER_ZOOM_GRAPHIC_CONTEXT_HPP
#define HEADER_ZOOM_GRAPHIC_CONTEXT_HPP

#include "vector2d.hpp"
#include "graphic_context.hpp"

/** A GraphicContext, that wraps another GraphicContext and adds
    coordinate translation like zoom and scrolling */
class ZoomGraphicContext : public GraphicContext
{
private:
  int x1;
  int y1;
  int x2;
  int y2;

  GraphicContext* parent_gc;

  float x_offset;
  float y_offset;
  float zoom;

  int translate_x (int);
  int translate_y (int);
public:
  ZoomGraphicContext ();
  ZoomGraphicContext (int x1_, int y1_, int x2_, int y2_);

  void set_clip_rect (int x1_, int y1_, int x2_, int y2_);

  /** Sets the clipping rectangles needed for the GC */
  void lock ();

  /** Resets the clipping to the old state */
  void unlock ();
  
  void draw_lines (std::vector<Line>& lines, Color color, int wide = 0);
  void draw_line(float x1, float y1, float x2, float y2, Color color, int wide = 0);
  void draw_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_circle(float x, float y, float r, Color color);
  void draw_circles(std::vector<Circle>& circles, Color color);
  void draw_fill_circle(float x, float y, float r, Color color);
  void draw_fill_rect(float x1, float y1, float x2, float y2, Color color);
  void draw_string(float x, float y, const std::string& str, Color color = Color (0xFFFFFFFF));
  void draw_string_centered(float x, float y, const std::string& str, Color color = Color (0xFFFFFFFF));

  int get_width ();
  int get_height ();

  void clear ()  { parent_gc->clear (); }

  /** FIXME: flip should be handled outsite of GraphicContext */
  void flip () { parent_gc->flip (); }

  void set_parent_gc (GraphicContext* gc);

  /** 
   * Zoom Out the GraphicContext by one 'unit', using the x and y
   * position as center.
   * 
   * @param x x in screen coordinates
   * @param y y in screen coordinates
   * 
   * @return true if zoom was successfull, false if zoom boundaries
   * have been reached
   */
  bool zoom_in (int x, int y);

  /** 
   * Zoom Into the GraphicContext by one 'unit', using the x and y
   * position as center.
   * 
   * @param x x in screen coordinates
   * @param y y in screen coordinates
   * 
   * @return true if zoom was successfull, false if zoom boundaries
   * have been reached
   */  
  bool zoom_out (int x, int y);
  
  /** 
   * Zoom into the given area, all coordinates in world units
   * 
   * @param x1 left border in world coordinates
   * @param y1 top border in world coordinates
   * @param x2 right border in world coordinates
   * @param y2 bottom border in world coordinates
   */
  void zoom_to (int x1, int y1, int x2, int y2);

  /** Move to the given x and y world coordinates to the center of the
      screen */
  void move_to (float x, float y);

  /** Allows a client application to make use of the partent GC in
      case it wants to draw elements which should not get scaled */
  GraphicContext* get_parent_gc () { return parent_gc; }

  /** Move the current position relativly x/y width */
  void translate_offset (int x, int y);

 /** FIXME: What exactly is an offset?! */
  void set_offset (float x, float y);

  /** FIXME: What exactly is an offset?! */
  float get_x_offset () { return x_offset; }

  /** FIXME: What exactly is an offset?! */
  float get_y_offset () { return y_offset; }

  /** @return current zoom factor in use */
  float get_zoom () const { return zoom; }

  /** Set the zoom factor, if zoom is to large/small false will
      returned and zoom will be set to lowest/highest possible
      value */
  bool  set_zoom (const float& z);

  /** Convert a coordinate from screen units to world units */
  float screen_to_world_x (float x);

  /** Convert a coordinate from screen units to world units */
  float screen_to_world_y (float y);

  /** Convert a coordinate from world units to screen units */
  float world_to_screen_x (float x);

  /** Convert a coordinate from world units to screen units */
  float world_to_screen_y (float y);

  /** Convert a coordinate from screen units to world units */
  Vector2d screen_to_world (const Vector2d&);

  /** Convert a coordinate from world units to screen units */
  Vector2d world_to_screen (const Vector2d&);

  void flip (int x1, int y1, int x2, int y2);

  void push_quick_draw() { parent_gc->push_quick_draw(); }
  void pop_quick_draw()  { parent_gc->pop_quick_draw(); }
};

#endif

/* EOF */
