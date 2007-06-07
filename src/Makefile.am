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

bindir=$(prefix)/games

bin_PROGRAMS = @ConstruoTargets@

EXTRA_PROGRAMS = construo.x11 construo.glut construo.freeglut

noinst_LIBRARIES = libconstruo.a

libconstruo_a_SOURCES = \
bounding_box.cpp \
bounding_box.hpp \
buttons.hpp \
collider.hpp \
color.hpp \
colors.cpp \
colors.hpp \
command_line.cpp \
command_line.hpp \
config.cpp \
config.hpp \
construo.cpp \
construo.hpp \
construo_error.hpp \
controller.cpp \
controller.hpp \
cursors/cursor_collider.xbm \
cursors/cursor_collider_mask.xbm \
cursors/cursor_insert.xbm \
cursors/cursor_insert_mask.xbm \
cursors/cursor_scroll.xbm \
cursors/cursor_scroll_mask.xbm \
cursors/cursor_select.xbm \
cursors/cursor_select_mask.xbm \
cursors/cursor_zoom.xbm \
cursors/cursor_zoom_mask.xbm \
cursors/cursors.hpp \
cursor_type.hpp \
delta_manager.hpp \
events.hpp \
force_emitter.hpp \
graphic_context.hpp \
gui_buttons.cpp \
gui_buttons.hpp \
gui_child_manager.cpp \
gui_child_manager.hpp \
gui_component.cpp \
gui_component.hpp \
gui_directory.cpp \
gui_directory.hpp \
gui_directory_button.cpp \
gui_directory_button.hpp \
gui_file_button.cpp \
gui_file_button.hpp \
gui_file_manager.cpp \
gui_file_manager.hpp \
gui_label.cpp \
gui_label.hpp \
gui_manager.cpp \
gui_manager.hpp \
gui_new_file_button.cpp \
gui_new_file_button.hpp \
gui_window.cpp \
gui_window.hpp \
input_context.cpp \
input_context.hpp \
keep_alive.hpp \
keep_alive_mgr.cpp \
keep_alive_mgr.hpp \
keys.hpp \
lisp_reader.cpp \
lisp_reader.hpp \
lisp_writer.cpp \
lisp_writer.hpp \
lispreader.cpp \
lispreader.hpp \
load_gui_manager.cpp \
load_gui_manager.hpp \
math.hpp \
particle.cpp \
particle.hpp \
particle_factory.cpp \
particle_factory.hpp \
path_manager.cpp \
path_manager.hpp \
rect.hpp \
rect_collider.cpp \
rect_collider.hpp \
root_graphic_context.cpp \
root_graphic_context.hpp \
save_gui_manager.cpp \
save_gui_manager.hpp \
screen_manager.cpp \
screen_manager.hpp \
selection.cpp \
selection.hpp \
settings.cpp \
settings.hpp \
spring.cpp \
spring.hpp \
spring_type.hpp \
string_utils.cpp \
string_utils.hpp \
system_context.hpp \
unix_system.cpp \
unix_system.hpp \
vector.cpp \
vector.hpp \
vector2d.hpp \
world.cpp \
world.hpp \
world_button.cpp \
world_button.hpp \
world_gui_manager.cpp \
world_gui_manager.hpp \
worldview_collider_tool.cpp \
worldview_collider_tool.hpp \
worldview_component.cpp \
worldview_component.hpp \
worldview_insert_tool.cpp \
worldview_insert_tool.hpp \
worldview_select_tool.cpp \
worldview_select_tool.hpp \
worldview_tool.cpp \
worldview_tool.hpp \
worldview_zoom_tool.cpp \
worldview_zoom_tool.hpp \
zoom_graphic_context.cpp \
zoom_graphic_context.hpp

construo_x11_SOURCES  = construo_main.cpp construo_main.hpp x11_display.hpp x11_display.cpp 
construo_x11_LDADD    = libconstruo.a @x11_LIBS@
construo_x11_CXXFLAGS = -DUSE_X11_DISPLAY @x11_CXXFLAGS@

construo_glut_SOURCES = construo_main.cpp construo_main.hpp glut_display.hpp glut_display.cpp
construo_glut_LDADD   = libconstruo.a @glut_LIBS@
construo_glut_CXXFLAGS = -DUSE_GLUT_DISPLAY -DHAVE_GLUT @glut_CXXFLAGS@

construo_freeglut_SOURCES = construo_main.cpp construo_main.hpp glut_display.hpp glut_display.cpp
construo_freeglut_LDADD   = libconstruo.a @freeglut_LIBS@
construo_freeglut_CXXFLAGS = -DUSE_GLUT_DISPLAY -DHAVE_FREEGLUT

# pkgdatadir = $(datadir)/games/@PACKAGE@

# EOF #
