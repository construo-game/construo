##  Construo - A wire-frame construction game
##  Copyright (C) 2002 Ingo Ruhnke <grumbel@gmx.de>
##
##  This program is free software; you can redistribute it and/or
##  modify it under the terms of the GNU General Public License
##  as published by the Free Software Foundation; either version 2
##  of the License, or (at your option) any later version.
## 
##  This program is distributed in the hope that it will be useful,
##  but WITHOUT ANY WARRANTY; without even the implied warranty of
##  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
##  GNU General Public License for more details.
##  
##  You should have received a copy of the GNU General Public License
##  along with this program; if not, write to the Free Software
##  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

SUBDIRS = examples

bindir=$(prefix)/games

bin_PROGRAMS = construo

construo_SOURCES = \
buttons.hxx   construo_error.hxx  graphic_context.hxx  keys.hxx            \
color.hxx     construo_main.cxx   input_context.cxx    particle.hxx        \
config.cxx    construo_main.hxx   input_context.hxx    spring.hxx           world.cxx       \
config.hxx    delta_manager.hxx   keep_alive.hxx       system_context.hxx  world.hxx       \
construo.cxx  events.hxx          keep_alive_mgr.cxx   unix_system.cxx     x11_display.cxx \
construo.hxx  force_emitter.hxx   keep_alive_mgr.hxx   unix_system.hxx     x11_display.hxx \
lispreader.c  lispreader.h       lisp_reader.hxx lisp_reader.cxx \
particle.cxx  spring.cxx  particle_factory.hxx particle_factory.cxx \
lisp_writer.hxx lisp_writer.cxx \
gui_manager.hxx gui_manager.cxx \
gui_component.hxx \
worldview_component.hxx worldview_component.cxx \
controller.hxx controller.cxx \
gui_buttons.hxx gui_buttons.cxx \
zoom_graphic_context.cxx zoom_graphic_context.hxx \
colors.hxx colors.cxx \
gui_window.hxx gui_window.cxx \
gui_component.cxx \
command_line.hxx command_line.cxx \
settings.hxx  settings.cxx \
vector2d.hxx \
math.hxx  \
worldview_tool.hxx worldview_tool.cxx \
worldview_select_tool.hxx worldview_select_tool.cxx \
worldview_insert_tool.hxx worldview_insert_tool.cxx

LDADD=-L/usr/X11R6/lib/ -lXext -lXxf86vm

# pkgdatadir = $(datadir)/games/@PACKAGE@
EXTRA_DIST= TODO xml2sexpr.sh

# EOF #