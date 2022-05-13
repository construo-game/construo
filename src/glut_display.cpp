// Construo - A wire-frame construction gamee
// Copyright (C) 2002 Ingo Ruhnke <grumbel@gmail.com>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <GL/freeglut.h>
#include <GL/freeglut_ucall.h>
#include <assert.h>
#include <iostream>
#include <stdio.h>
#include <string.h>

#include <logmich/log.hpp>

#include "buttons.hpp"
#include "events.hpp"
#include "settings.hpp"
#include "screen_manager.hpp"
#include "glut_display.hpp"

GlutDisplay::GlutDisplay(std::string const& title, int width, int height, int fullscreen) :
  m_window_x_pos(),
  m_window_y_pos(),
  m_window_width(),
  m_window_height(),
  m_width(width),
  m_height(height),
  m_mouse_x(),
  m_mouse_y(),
  m_block(),
  m_update_display(0),
  m_is_fullscreen()
{
  int argc = 1;
  char* argv[2];
  argv[0] = strdup("construo");
  argv[1] = NULL;
  glutInit(&argc, argv); // Only pass empty dummies
  free(argv[0]);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(m_width, m_height);
  //glutInitWindowPosition(100, 100); don't care
  glutSetWindow(glutCreateWindow(title.c_str()));

  glutDisplayFuncUcall([](void* userdata) {
     static_cast<GlutDisplay*>(userdata)->display_func();
  }, this);

  glutReshapeFuncUcall([](int w, int h, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->reshape_func(w, h);
  }, this);

  glutMouseFuncUcall([](int button, int button_state, int x, int y, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->mouse_func(button, button_state, x, y);
  }, this);

  glutMotionFuncUcall([](int x, int y, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->mouse_motion_func(x, y);
  }, this);

  glutPassiveMotionFuncUcall([](int x, int y, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->mouse_motion_func(x, y);
  }, this);

  glutIdleFuncUcall([](void* userdata) {
    static_cast<GlutDisplay*>(userdata)->idle_func();
  }, this);

  glutKeyboardFuncUcall([](unsigned char key, int x, int y, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->keyboard_func(key, x, y);
  }, this);

  glutSpecialFuncUcall([](int key, int x, int y, void* userdata) {
    static_cast<GlutDisplay*>(userdata)->special_func(key, x, y);
  }, this);

  glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
  if (g_settings.alphablending)
  {
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  if (g_settings.antialiasing && g_settings.alphablending)
  {
    glEnable(GL_LINE_SMOOTH);
  }

  glutSetCursor(GLUT_CURSOR_FULL_CROSSHAIR);

  m_window_x_pos = 0;
  m_window_y_pos = 0;
  m_window_width  = width;
  m_window_height = height;

  if (fullscreen) {
    enter_fullscreen();
  }
}

GlutDisplay::~GlutDisplay()
{
}

void
GlutDisplay::run()
{
  glutMainLoop();
}

void
GlutDisplay::draw_lines(std::vector<Line>& lines, Color color, int wide)
{
  if (g_settings.thick_lines) {
    glLineWidth(static_cast<float>(wide));
  }

  glBegin(GL_LINES);
  for (std::vector<Line>::const_iterator i = lines.begin(); i != lines.end(); ++i)
  {
    glVertex2f(i->x1, i->y1);
    glVertex2f(i->x2, i->y2);
  }
  glEnd();
}

void
GlutDisplay::draw_line(float x1, float y1, float x2, float y2, Color color, int wide)
{
  if (g_settings.thick_lines)
    glLineWidth(static_cast<float>(wide));

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

void
GlutDisplay::draw_rect(float x1, float y1, float x2, float y2, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(2);

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_LINE_STRIP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);
  glVertex2f(x1, y1);
  glEnd();
}

void
GlutDisplay::draw_fill_rect(float x1, float y1, float x2, float y2, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(.5f);

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_QUADS);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);
  glEnd();
}

void
GlutDisplay::draw_circles(std::vector<Circle>& circles, Color color)
{
  for (auto i = circles.begin(); i != circles.end(); ++i)
  {
    draw_circle(i->x, i->y, i->r, color);
  }
}

void
GlutDisplay::draw_circle(float x, float y, float r, Color color)
{
  glColor4f(color.r, color.g, color.b, color.a);

  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
  //gluQuadricNormals(qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef(x, y, 0);
  gluDisk(qobj, 0, r, 8, 1);
  /*draw_rect(x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix();
  gluDeleteQuadric(qobj);
}

void
GlutDisplay::draw_fill_circle(float x, float y, float r, Color color)
{
  glColor4f(color.r, color.g, color.b, color.a);
  //draw_fill_rect(x - r, y - r, x + r, y + r,
  //              color);

  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_FILL);
  //gluQuadricNormals(qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef(x, y, 0);
  gluDisk(qobj, 0, r, 8, 1);
  /*draw_rect(x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix();
  gluDeleteQuadric(qobj);
}

void
GlutDisplay::draw_string(float x, float y, const std::string& str, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(1.0f);

  glColor4f(color.r, color.g, color.b, color.a);
  glPushMatrix();
  glTranslatef(x , y, 0);
  glScalef(.07f, -.07f, 0);

  for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
  {
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *i);
    //glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, *i);
  }
  glPopMatrix();
}


void
GlutDisplay::draw_string_centered(float x, float y, const std::string& str, Color color)
{
  draw_string(x - (7.5f * static_cast<float>(str.length())) / 2.0f,
              y, str, color);
}

bool
GlutDisplay::get_key(int key)
{
  return false;
}

float
GlutDisplay::get_mouse_x()
{
  return static_cast<float>(m_mouse_x);
}

float
GlutDisplay::get_mouse_y()
{
  return static_cast<float>(m_mouse_y);
}

void
GlutDisplay::clear()
{
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GlutDisplay::flip()
{
  glutSwapBuffers();
}

void
GlutDisplay::reshape_func(int w, int h)
{
  glViewport(0,0, w, h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, w, h, 0);

  glMatrixMode(GL_MODELVIEW);
  m_width  = w;
  m_height = h;

  glScissor(0, 0, m_width, m_height);

  ScreenManager::instance()->set_geometry(0, 0, static_cast<float>(m_width), static_cast<float>(m_height));
}

void
GlutDisplay::display_func()
{
  ScreenManager::instance()->run_once(*this);
}

void
GlutDisplay::mouse_func(int button, int button_state, int x, int y)
{
  m_mouse_x = x;
  m_mouse_y = y;

  Event event;
  event.type = BUTTON_EVENT;

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
      event.button.id = BUTTON_ZOOM_IN;
      break;
    case 4:
      event.button.id = BUTTON_ZOOM_OUT;
      break;
    default:
      log_debug("GlutDisplay: Unhandle mouse button press: {} {}", button, button_state);
      return;
  }
  events.push(event);
}

void
GlutDisplay::idle_func()
{
  /*  if (Controller::instance()->is_running() || update_display > 0)
      {
      //system_context->sleep(0); // limit CPU usage via brute force
      update_display = 0;
      }*/
  if (!ScreenManager::instance()->is_finished())
  {
    ScreenManager::instance()->run_once(*this);
  }
  else
  {
    glutLeaveMainLoop();
  }
}

void
GlutDisplay::special_func(int key, int x, int y)
{
  switch (key)
  {
    case GLUT_KEY_F11:
      if (m_is_fullscreen)
        leave_fullscreen();
      else
        enter_fullscreen();
      break;
  }
}

void
GlutDisplay::keyboard_func(unsigned char key, int x, int y)
{
  Event event;
  event.type = BUTTON_EVENT;
  event.button.pressed = true;

  switch (key)
  {
    case 127: // Delete
      event.button.id = BUTTON_DELETE;
      break;
    case 32: // Space
      event.button.id = BUTTON_RUN;
      break;
    case 9: // Tab
      event.button.id = BUTTON_TOGGLESLOWMO;
      break;
    case 27: // Escape
    case 'q':
      event.button.id = BUTTON_ESCAPE;
      break;
    case 'h':
      event.button.id = BUTTON_FLIP;
      break;
    case 'f':
      event.button.id = BUTTON_FIX;
      break;
    case 'd':
      event.button.id = BUTTON_DUPLICATE;
      break;

    case 'v':
      event.button.id = BUTTON_SETVELOCITY;
      break;

    case 'c':
      event.button.id = BUTTON_CLEAR;
      break;

    case 'j':
      event.button.id = BUTTON_JOIN;
      break;

    case 's':
      event.button.id = BUTTON_SCALE;
      break;

    case 'a':
      event.button.id = BUTTON_ACTIONCAM;
      break;

    case 'o':
      event.button.id = BUTTON_HIDEDOTS;
      break;

    case '!':
      event.button.id = BUTTON_QUICKLOAD1;
      break;

    case '@':
      event.button.id = BUTTON_QUICKLOAD2;
      break;

    case '#':
      event.button.id = BUTTON_QUICKLOAD3;
      break;

    case '$':
      event.button.id = BUTTON_QUICKLOAD4;
      break;

    case '%':
      event.button.id = BUTTON_QUICKLOAD5;
      break;

    case '^':
      event.button.id = BUTTON_QUICKLOAD6;
      break;

    case '&':
      event.button.id = BUTTON_QUICKLOAD7;
      break;

    case '*':
      event.button.id = BUTTON_QUICKLOAD8;
      break;

    case '(':
      event.button.id = BUTTON_QUICKLOAD9;
      break;

    case ')':
      event.button.id = BUTTON_QUICKLOAD0;
      break;

    case '0':
      event.button.id = BUTTON_QUICKSAVE0;
      break;

    case '1':
      event.button.id = BUTTON_QUICKSAVE1;
      break;

    case '2':
      event.button.id = BUTTON_QUICKSAVE2;
      break;

    case '3':
      event.button.id = BUTTON_QUICKSAVE3;
      break;

    case '4':
      event.button.id = BUTTON_QUICKSAVE4;
      break;

    case '5':
      event.button.id = BUTTON_QUICKSAVE5;
      break;

    case '6':
      event.button.id = BUTTON_QUICKSAVE6;
      break;

    case '7':
      event.button.id = BUTTON_QUICKSAVE7;
      break;

    case '8':
      event.button.id = BUTTON_QUICKSAVE8;
      break;

    case '9':
      event.button.id = BUTTON_QUICKSAVE9;
      break;

    case 'g':
      event.button.id = BUTTON_GRID;
      break;

    case 'u':
      event.button.id = BUTTON_UNDO;
      break;
    case 'r':
      event.button.id = BUTTON_REDO;
      break;
    case '+':
    case '=': // so that people don't have to press shift
      event.button.id = BUTTON_ZOOM_IN;
      break;
    case '-':
      event.button.id = BUTTON_ZOOM_OUT;
      break;
    default:
      log_debug("GlutDisplay: Unhandled keypress: '{}' {}+{}", key, x, y);
      return;
  }

  events.push(event);
}

void
GlutDisplay::mouse_motion_func(int x, int y)
{
  m_mouse_x = x;
  m_mouse_y = y;
}

void
GlutDisplay::leave_fullscreen()
{
  log_debug("GlutDisplay: leaving fullscreen: restoring to: pos: {}x{}+{}+{}",
            m_window_width, m_window_height,
            m_window_x_pos, m_window_y_pos);

  glutReshapeWindow(m_window_width, m_window_height);
  glutPositionWindow(m_window_x_pos, m_window_y_pos);

  m_is_fullscreen = false;
}

void
GlutDisplay::enter_fullscreen()
{
#if 0
  char mode[64];
  snprintf(mode, 64, "%dx%d:%d@%d", width, height, 16, 80);
  log_debug("GlutDisplay: switching to: {}", mode);
  glutGameModeString(mode);
  glutEnterGameMode();
  is_fullscreen = true;
#else
  log_debug("GlutDisplay: Entering fullscreen");

  m_window_x_pos  = glutGet((GLenum)GLUT_WINDOW_X);
  m_window_y_pos  = glutGet((GLenum)GLUT_WINDOW_Y);
  m_window_width  = glutGet((GLenum)GLUT_WINDOW_WIDTH);
  m_window_height = glutGet((GLenum)GLUT_WINDOW_HEIGHT);

  log_debug("Saving window: {}x{}+{}+{}",
            m_window_width, m_window_height,
            m_window_x_pos, m_window_y_pos);

  glutFullScreen();

  m_is_fullscreen = true;
#endif
}

void
GlutDisplay::set_clip_rect(float x1, float y1, float x2, float y2)
{
  glEnable(GL_SCISSOR_TEST);
  glScissor(
    static_cast<int>(x1), // x
    m_height - 1 - static_cast<int>(y2), // y
    static_cast<int>(x2 - x1), // weight
    static_cast<int>(y2 - y1) // height
    );
}

void
GlutDisplay::clear_clip_rect()
{
  glDisable(GL_SCISSOR_TEST);
}

void
GlutDisplay::push_quick_draw()
{
  if (g_settings.antialiasing && g_settings.alphablending)
  {
    glDisable(GL_LINE_SMOOTH);
  }
}

void
GlutDisplay::pop_quick_draw()
{
  if (g_settings.antialiasing && g_settings.alphablending)
  {
    glEnable(GL_LINE_SMOOTH);
  }
}

void
GlutDisplay::set_cursor_real(CursorType cursor)
{
  switch(cursor)
  {
    case CURSOR_INSERT:
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      break;
    case CURSOR_SCROLL:
      glutSetCursor(GLUT_CURSOR_INFO);
      break;
    case CURSOR_ZOOM:
      glutSetCursor(GLUT_CURSOR_HELP);
      break;
    case CURSOR_COLLIDER:
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      break;
    case CURSOR_SELECT:
      glutSetCursor(GLUT_CURSOR_INHERIT);
      break;
    case CURSOR_ROTATE:
      glutSetCursor(GLUT_CURSOR_CYCLE);
      break;
    default:
      log_debug("GlutDisplay: Unhandled cursor type: {}", static_cast<int>(cursor));
      break;
  }

}

/* EOF */
