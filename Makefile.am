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

bin_PROGRAMS = @ConstruoTargets@

EXTRA_PROGRAMS = construo.x11 construo.glut construo.freeglut

noinst_LIBRARIES = libconstruo.a

libconstruo_a_SOURCES = \
bounding_box.cxx \
bounding_box.hxx \
buttons.hxx \
collider.hxx \
color.hxx \
colors.cxx \
colors.hxx \
command_line.cxx \
command_line.hxx \
config.cxx \
config.hxx \
construo.cxx \
construo.hxx \
construo_error.hxx \
controller.cxx \
controller.hxx \
cursor_collider.xbm \
cursor_collider_mask.xbm \
cursor_insert.xbm \
cursor_insert_mask.xbm \
cursor_scroll.xbm \
cursor_scroll_mask.xbm \
cursor_select.xbm \
cursor_select_mask.xbm \
cursor_zoom.xbm \
cursor_zoom_mask.xbm \
cursors.hxx \
delta_manager.hxx \
events.hxx \
force_emitter.hxx \
globals.cxx \
globals.hxx \
graphic_context.hxx \
gui_buttons.cxx \
gui_buttons.hxx \
gui_child_manager.cxx \
gui_child_manager.hxx \
gui_component.cxx \
gui_component.hxx \
gui_directory.cxx \
gui_directory.hxx \
gui_directory_button.cxx \
gui_directory_button.hxx \
gui_file_button.cxx \
gui_file_button.hxx \
gui_file_manager.cxx \
gui_file_manager.hxx \
gui_label.cxx \
gui_label.hxx \
gui_manager.cxx \
gui_manager.hxx \
gui_new_file_button.cxx \
gui_new_file_button.hxx \
gui_window.cxx \
gui_window.hxx \
input_context.cxx \
input_context.hxx \
keep_alive.hxx \
keep_alive_mgr.cxx \
keep_alive_mgr.hxx \
keys.hxx \
lisp_reader.cxx \
lisp_reader.hxx \
lisp_writer.cxx \
lisp_writer.hxx \
lispreader.cxx \
lispreader.hxx \
load_gui_manager.cxx \
load_gui_manager.hxx \
math.hxx \
particle.cxx \
particle.hxx \
particle_factory.cxx \
particle_factory.hxx \
path_manager.cxx \
path_manager.hxx \
rect.hxx \
rect_collider.cxx \
rect_collider.hxx \
root_graphic_context.cxx \
root_graphic_context.hxx \
save_gui_manager.cxx \
save_gui_manager.hxx \
screen_manager.cxx \
screen_manager.hxx \
selection.cxx \
selection.hxx \
settings.cxx \
settings.hxx \
spring.cxx \
spring.hxx \
spring_type.hxx \
string_utils.cxx \
string_utils.hxx \
system_context.hxx \
unix_system.cxx \
unix_system.hxx \
vector.cxx \
vector.hxx \
vector2d.hxx \
world.cxx \
world.hxx \
world_button.cxx \
world_button.hxx \
world_gui_manager.cxx \
world_gui_manager.hxx \
worldview_collider_tool.cxx \
worldview_collider_tool.hxx \
worldview_component.cxx \
worldview_component.hxx \
worldview_insert_tool.cxx \
worldview_insert_tool.hxx \
worldview_select_tool.cxx \
worldview_select_tool.hxx \
worldview_tool.cxx \
worldview_tool.hxx \
worldview_zoom_tool.cxx \
worldview_zoom_tool.hxx \
zoom_graphic_context.cxx \
zoom_graphic_context.hxx

construo_x11_SOURCES  = construo_main.cxx construo_main.hxx x11_display.hxx x11_display.cxx 
construo_x11_LDADD    = libconstruo.a @x11_LIBS@
construo_x11_CXXFLAGS = -DUSE_X11_DISPLAY @x11_CXXFLAGS@

construo_glut_SOURCES = construo_main.cxx construo_main.hxx glut_display.hxx glut_display.cxx
construo_glut_LDADD   = libconstruo.a @glut_LIBS@
construo_glut_CXXFLAGS = -DUSE_GLUT_DISPLAY -DHAVE_GLUT

construo_freeglut_SOURCES = construo_main.cxx construo_main.hxx glut_display.hxx glut_display.cxx
construo_freeglut_LDADD   = libconstruo.a @freeglut_LIBS@
construo_freeglut_CXXFLAGS = -DUSE_GLUT_DISPLAY -DHAVE_FREEGLUT

# pkgdatadir = $(datadir)/games/@PACKAGE@
EXTRA_DIST = TODO  INSTALL.configure xml2sexpr.sh compatlib/sstream

# EOF #