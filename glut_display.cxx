//  $Id$
//
//  Construo - A wire-frame construction gamee
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

#ifdef HAVE_FREEGLUT
#  include <GL/freeglut.h>
#else
#  include <GL/glut.h>
#endif

#include <iostream>
#include "buttons.hxx"
#include "events.hxx"
#include "settings.hxx"
#include "gui_manager.hxx"
#include "glut_display.hxx"

GlutDisplay* GlutDisplay::instance_ = 0;

void reshape_func(int w, int h)
{
  GlutDisplay::instance()->reshape_func(w, h);
}

void display_func ()
{
  GlutDisplay::instance()->display_func();
}

void mouse_func (int button, int button_state, int x, int y)
{
  GlutDisplay::instance()->mouse_func(button, button_state, x, y);
}

void idle_func ()
{
  GlutDisplay::instance()->idle_func();
}

void keyboard_func (unsigned char key, int x, int y)
{
  GlutDisplay::instance()->keyboard_func(key, x, y);
}

void mouse_motion_func (int x, int y)
{
  GlutDisplay::instance()->mouse_motion_func(x, y);
}

GlutDisplay::GlutDisplay (int w, int h)
{
  instance_ = this;

  update_display = 0;
  width  = w;
  height = h;

  int argc = 1;
  char* argv[] = { "construo", "\0" };

  glutInit(&argc, argv); // Only pass empty dummies
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(width, height);
  //glutInitWindowPosition(100, 100); don't care
  glutCreateWindow(argv[0]);

  glutDisplayFunc(::display_func);
  glutReshapeFunc(::reshape_func);
  glutMouseFunc(::mouse_func);

  glutMotionFunc (::mouse_motion_func);
  glutPassiveMotionFunc (::mouse_motion_func);

  glutIdleFunc (::idle_func);
  glutKeyboardFunc(::keyboard_func);
  
  glClearColor (0.0, 0.0, 0.0, 0.1);
  if (settings.alphablending)
    {
      glShadeModel (GL_SMOOTH);
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

  if (settings.antialiasing && settings.alphablending)
    {
      glEnable(GL_LINE_SMOOTH);
    }
}

void
GlutDisplay::run()
{
  std::cout << "Starting glut mainloop" << std::endl;
  glutMainLoop(); 
  std::cout << "Ending glut mainloop" << std::endl;
}

GlutDisplay::~GlutDisplay()
{
}

void 
GlutDisplay::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  glLineWidth (wide);
  glColor4f (color.r, color.g, color.b, color.a);
  glBegin (GL_LINES);
  glVertex2f (x1, y1);
  glVertex2f (x2, y2);
  glEnd ();
}

void
GlutDisplay::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  glLineWidth (2);
  glColor4f (color.r, color.g, color.b, color.a);
  glBegin (GL_LINE_STRIP);
  glVertex2f (x1, y1);
  glVertex2f (x2, y1);
  glVertex2f (x2, y2);
  glVertex2f (x1, y2);
  glVertex2f (x1, y1);
  glEnd ();
}

void
GlutDisplay::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  glLineWidth (.5f);
  glColor4f (color.r, color.g, color.b, color.a);
  glBegin (GL_QUADS);
  glVertex2f (x1, y1);
  glVertex2f (x2, y1);
  glVertex2f (x2, y2);
  glVertex2f (x1, y2);
  glEnd ();
}

void
GlutDisplay::draw_circle(float x, float y, float r, Color color)
{
  glColor4f (color.r, color.g, color.b, color.a);
  GLUquadricObj* qobj = gluNewQuadric ();
  //gluQuadricNormals (qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef (x, y, 0);
  gluDisk (qobj, 0, r, 8, 1);
  /*draw_rect (x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix ();
  gluDeleteQuadric (qobj);
}

void
GlutDisplay::draw_fill_circle(float x, float y, float r, Color color)
{
  glColor4f (color.r, color.g, color.b, color.a);
  //draw_fill_rect (x - r, y - r, x + r, y + r,
  //              color);
  
  GLUquadricObj* qobj = gluNewQuadric ();
  //gluQuadricNormals (qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef (x, y, 0);
  gluDisk (qobj, 0, r, 8, 1);
  /*draw_rect (x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix ();
  gluDeleteQuadric (qobj);
}

void
GlutDisplay::draw_string(float x, float y, const std::string& str, Color color)
{
  glLineWidth (1.0f);
  glColor4f (color.r, color.g, color.b, color.a);
  glPushMatrix();
  glTranslatef (x , y, 0);
  glScalef (.07f, -.07, 0);

  for (std::string::const_iterator i = str.begin (); i != str.end (); ++i)
    {
#ifdef HAVE_FREEGLUT
      glutBitmapCharacter  (GLUT_BITMAP_8_BY_13, *i);
#else
      glutStrokeCharacter  (GLUT_STROKE_MONO_ROMAN, *i);
      //glutStrokeWidth (GLUT_STROKE_MONO_ROMAN, *i);
#endif
    }
  glPopMatrix();
}


bool
GlutDisplay::get_key (int key)
{
  return false;
}

int
GlutDisplay::get_mouse_x ()
{
  return mouse_x;
}

int
GlutDisplay::get_mouse_y ()
{
  return mouse_y;
}

void
GlutDisplay::clear ()
{
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GlutDisplay::flip ()
{
  glutSwapBuffers();
}

void
GlutDisplay::flip (int x1, int y1, int x2, int y2)
{
  assert (false);
}


void
GlutDisplay::reshape_func(int w, int h)
{
  glViewport (0,0, w, h);
  glLoadIdentity();
  gluOrtho2D (0, w, h, 0);
}

void
GlutDisplay::display_func ()
{
  GUIManager::instance()->run_once();
}

void
GlutDisplay::mouse_func (int button, int button_state, int x, int y)
{
  mouse_x = x;
  mouse_y = y;

  Event event;
  event.type = BUTTON_EVENT;
  
  //std::cout << "mouse button press: " << button << " " << button_state <<  " " << x << " " << y << std::endl;

  if (button_state == 0)
    event.button.pressed = true;
  else
    event.button.pressed = false;

  switch (button)
    {
    case 0:
      event.button.id = BUTTON_PRIMARY;
      break;
    case 1:
      event.button.id = BUTTON_TERTIARY;
      break;
    case 2:
      event.button.id = BUTTON_SECONDARY;
      break;
    case 3:
      event.button.id = BUTTON_ZOOM_OUT;
      break;
    case 4:
      event.button.id = BUTTON_ZOOM_IN;
      break;
    default:
      std::cout << "GlutDisplay: Unhandle mouse button press: " << button << " " << button_state << std::endl;
      return;
    }
  events.push (event);
}

void
GlutDisplay::idle_func ()
{
  /*  if (Controller::instance()->is_running() || update_display > 0)
    {
      //system_context->sleep (0); // limit CPU usage via brute force
      update_display = 0;
    }*/
  GUIManager::instance ()->run_once();
}

void
GlutDisplay::keyboard_func (unsigned char key, int x, int y)
{
  //std::cout << "keypress: " << int(key) << " " << x << " " << y << std::endl;

  Event event;
  event.type = BUTTON_EVENT;
  event.button.pressed = true;

  switch (key)
    {
    case 127: // Delete
      event.button.id = BUTTON_DELETE;
      break;
    case 27: // Escape
    case 'q':
      event.button.id = BUTTON_ESCAPE;
      break;
    case 'f':
      set_fullscreen(settings.fullscreen);
      break;
    case '0':
      event.button.id = BUTTON_QUICKSAVE0;
      break;
    case 'u':
      event.button.id = BUTTON_UNDO;
      break;
    case 'r':
      event.button.id = BUTTON_REDO;
      break;
    default:
      std::cout << "GlutDisplay: Unhandled keypress: " << int(key) << " " << x << " " << y << std::endl;
      return;
    }

  events.push(event); 
}

void
GlutDisplay::mouse_motion_func (int x, int y)
{
  //std::cout << "Motion: " << x << " " << y << std::endl;
  mouse_x = x;
  mouse_y = y;
}

void
GlutDisplay::set_fullscreen (bool fullscreen)
{
  if (fullscreen)
    {        
      char mode[64];
      snprintf (mode, 64, "%dx%d:%d@%d", width, height, 16, 80);
      std::cout << "GlutDisplay: switching to: " << mode << std::endl;
      glutGameModeString(mode);
      glutEnterGameMode();
    }
  else
    {
    }
}

/* EOF */
