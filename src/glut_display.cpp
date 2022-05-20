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
#include <geom/rect.hpp>

#include "action.hpp"
#include "events.hpp"
#include "settings.hpp"
#include "screen_manager.hpp"
#include "glut_display.hpp"

namespace construo {

namespace {

void init_default_keybindings(GlutDisplay& dpy)
{
  dpy.bind_key(127, Action::DELETE);
  dpy.bind_key(32, Action::RUN);
  dpy.bind_key(9, Action::TOGGLESLOWMO);
  dpy.bind_key(27, Action::ESCAPE);
  dpy.bind_key('q', Action::ESCAPE);
  dpy.bind_key('h', Action::FLIP);
  dpy.bind_key('f', Action::FIX);
  dpy.bind_key('d', Action::DUPLICATE);
  dpy.bind_key('v', Action::SETVELOCITY);
  dpy.bind_key('c', Action::CLEAR);
  dpy.bind_key('j', Action::JOIN);
  dpy.bind_key('s', Action::SCALE);
  dpy.bind_key('a', Action::ACTIONCAM);
  dpy.bind_key('o', Action::HIDEDOTS);
  dpy.bind_key('!', Action::QUICKLOAD1);
  dpy.bind_key('@', Action::QUICKLOAD2);
  dpy.bind_key('#', Action::QUICKLOAD3);
  dpy.bind_key('$', Action::QUICKLOAD4);
  dpy.bind_key('%', Action::QUICKLOAD5);
  dpy.bind_key('^', Action::QUICKLOAD6);
  dpy.bind_key('&', Action::QUICKLOAD7);
  dpy.bind_key('*', Action::QUICKLOAD8);
  dpy.bind_key('(', Action::QUICKLOAD9);
  dpy.bind_key(')', Action::QUICKLOAD0);
  dpy.bind_key('0', Action::QUICKSAVE0);
  dpy.bind_key('1', Action::QUICKSAVE1);
  dpy.bind_key('2', Action::QUICKSAVE2);
  dpy.bind_key('3', Action::QUICKSAVE3);
  dpy.bind_key('4', Action::QUICKSAVE4);
  dpy.bind_key('5', Action::QUICKSAVE5);
  dpy.bind_key('6', Action::QUICKSAVE6);
  dpy.bind_key('7', Action::QUICKSAVE7);
  dpy.bind_key('8', Action::QUICKSAVE8);
  dpy.bind_key('9', Action::QUICKSAVE9);
  dpy.bind_key('g', Action::GRID);
  dpy.bind_key('u', Action::UNDO);
  dpy.bind_key('r', Action::REDO);
  dpy.bind_key('+', Action::ZOOM_IN);
  dpy.bind_key('=', Action::ZOOM_IN);
  dpy.bind_key('-', Action::ZOOM_OUT);
}

} // namespace

GlutDisplay::GlutDisplay(std::string const& title, int width, int height, int fullscreen) :
  m_window_geometry(),
  m_size(width, height),
  m_mouse_pos(),
  m_block(),
  m_update_display(0),
  m_is_fullscreen(),
  m_key_bindings()
{
  int argc = 1;
  char* argv[2];
  argv[0] = strdup("construo");
  argv[1] = NULL;
  glutInit(&argc, argv); // Only pass empty dummies
  free(argv[0]);

  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(m_size.width(), m_size.height());
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

  m_window_geometry = geom::irect(geom::ipoint(0, 0), geom::isize(width, height));

  if (fullscreen) {
    enter_fullscreen();
  }

  init_default_keybindings(*this);
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
    glVertex2f(i->p1.x(), i->p1.y());
    glVertex2f(i->p2.x(), i->p2.y());
  }
  glEnd();
}

void
GlutDisplay::draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide)
{
  if (g_settings.thick_lines) {
    glLineWidth(static_cast<float>(wide));
  }

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_LINES);
  glVertex2f(p1.x(), p1.y());
  glVertex2f(p2.x(), p2.y());
  glEnd();
}

void
GlutDisplay::draw_rect(geom::frect const& rect, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(2);

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_LINE_STRIP);
  glVertex2f(rect.left(), rect.top());
  glVertex2f(rect.right(), rect.top());
  glVertex2f(rect.right(), rect.bottom());
  glVertex2f(rect.left(), rect.bottom());
  glVertex2f(rect.left(), rect.top());
  glEnd();
}

void
GlutDisplay::draw_fill_rect(geom::frect const& rect, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(.5f);

  glColor4f(color.r, color.g, color.b, color.a);
  glBegin(GL_QUADS);
  glVertex2f(rect.left(), rect.top());
  glVertex2f(rect.right(), rect.top());
  glVertex2f(rect.right(), rect.bottom());
  glVertex2f(rect.left(), rect.bottom());
  glVertex2f(rect.left(), rect.top());
  glEnd();
}

void
GlutDisplay::draw_circles(std::vector<Circle>& circles, Color color)
{
  for (auto i = circles.begin(); i != circles.end(); ++i)
  {
    draw_circle(i->pos, i->r, color);
  }
}

void
GlutDisplay::draw_circle(geom::fpoint const& pos, float r, Color color)
{
  glColor4f(color.r, color.g, color.b, color.a);

  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_SILHOUETTE);
  //gluQuadricNormals(qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0);
  gluDisk(qobj, 0, r, 8, 1);
  /*draw_rect(x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix();
  gluDeleteQuadric(qobj);
}

void
GlutDisplay::draw_fill_circle(geom::fpoint const& pos, float r, Color color)
{
  glColor4f(color.r, color.g, color.b, color.a);
  //draw_fill_rect(x - r, y - r, x + r, y + r,
  //              color);

  GLUquadricObj* qobj = gluNewQuadric();
  gluQuadricDrawStyle(qobj, GLU_FILL);
  //gluQuadricNormals(qobj, GLU_FLAT);
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0);
  gluDisk(qobj, 0, r, 8, 1);
  /*draw_rect(x - r, y - r, x + r, y + r,
    color);*/
  glPopMatrix();
  gluDeleteQuadric(qobj);
}

void
GlutDisplay::draw_string(geom::fpoint const& pos, const std::string& str, Color color)
{
  if (g_settings.thick_lines)
    glLineWidth(1.0f);

  glColor4f(color.r, color.g, color.b, color.a);
  glPushMatrix();
  glTranslatef(pos.x(), pos.y(), 0);
  glScalef(.07f, -.07f, 0);

  for (std::string::const_iterator i = str.begin(); i != str.end(); ++i)
  {
    glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, *i);
    //glutStrokeWidth(GLUT_STROKE_MONO_ROMAN, *i);
  }
  glPopMatrix();
}


void
GlutDisplay::draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color)
{
  draw_string(geom::fpoint(pos.x() - (7.5f * static_cast<float>(str.length())) / 2.0f,
                           pos.y()),
              str, color);
}

geom::fpoint
GlutDisplay::get_mouse_pos() const
{
  return geom::fpoint(m_mouse_pos);
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
  m_size = geom::isize(w, h);

  glScissor(0, 0, m_size.width(), m_size.height());

  ScreenManager::instance()->set_geometry(geom::fsize(m_size));
}

void
GlutDisplay::display_func()
{
  ScreenManager::instance()->run_once(*this);
}

void
GlutDisplay::mouse_func(int button, int button_state, int x, int y)
{
  m_mouse_pos = geom::ipoint(x, y);

  Event event;
  event.type = BUTTON_EVENT;

  if (button_state == 0)
    event.button.pressed = true;
  else
    event.button.pressed = false;

  switch (button)
  {
    case 0:
      event.button.id = Action::PRIMARY;
      break;
    case 1:
      event.button.id = Action::TERTIARY;
      break;
    case 2:
      event.button.id = Action::SECONDARY;
      break;
    case 3:
      event.button.id = Action::ZOOM_IN;
      break;
    case 4:
      event.button.id = Action::ZOOM_OUT;
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
      {
        Event event;
        event.type = BUTTON_EVENT;
        event.button.pressed = true;
        event.button.id = Action::FULLSCREEN;

        events.push(event);
      }
      break;
  }
}

void
GlutDisplay::keyboard_func(unsigned char key, int x, int y)
{
  if (auto const it = m_key_bindings.find(key); it == m_key_bindings.end()) {
    log_debug("GlutDisplay: Unhandled keypress: '{}' {}+{}", key, x, y);
  } else {
    Event event;
    event.type = BUTTON_EVENT;
    event.button.pressed = true;
    event.button.id = it->second;

    events.push(event);
  }
}

void
GlutDisplay::mouse_motion_func(int x, int y)
{
  m_mouse_pos = geom::ipoint(x, y);
}

void
GlutDisplay::bind_key(unsigned char key, Action action)
{
  if (auto it = m_key_bindings.find(key); it != m_key_bindings.end()) {
    log_warn("key already bound: {}", static_cast<int>(key));
  }

  m_key_bindings[key] = action;
}

void
GlutDisplay::leave_fullscreen()
{
  log_debug("GlutDisplay: leaving fullscreen: restoring to: pos: {}x{}+{}+{}",
            m_window_geometry.width(), m_window_geometry.height(),
            m_window_geometry.left(), m_window_geometry.top());

  glutReshapeWindow(m_window_geometry.width(), m_window_geometry.height());
  glutPositionWindow(m_window_geometry.left(), m_window_geometry.top());

  m_is_fullscreen = false;
}

void
GlutDisplay::toggle_fullscreen()
{
  if (m_is_fullscreen) {
    leave_fullscreen();
  } else {
    enter_fullscreen();
  }
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

  m_window_geometry = geom::irect(geom::ipoint(glutGet(static_cast<GLenum>(GLUT_WINDOW_X)),
                                               glutGet(static_cast<GLenum>(GLUT_WINDOW_Y))),
                                  geom::isize(glutGet(static_cast<GLenum>(GLUT_WINDOW_WIDTH)),
                                              glutGet(static_cast<GLenum>(GLUT_WINDOW_HEIGHT))));

  log_debug("Saving window: {}x{}+{}+{}",
            m_window_geometry.width(), m_window_geometry.height(),
            m_window_geometry.left(), m_window_geometry.top());

  glutFullScreen();

  m_is_fullscreen = true;
#endif
}

void
GlutDisplay::set_clip_rect(geom::frect const& rect)
{
  glEnable(GL_SCISSOR_TEST);
  glScissor(
    static_cast<int>(rect.left()), // x
    m_size.height() - 1 - static_cast<int>(rect.bottom()), // y
    static_cast<int>(rect.width()), // weight
    static_cast<int>(rect.height()) // height
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
    case CursorType::INSERT:
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      break;
    case CursorType::SCROLL:
      glutSetCursor(GLUT_CURSOR_INFO);
      break;
    case CursorType::ZOOM:
      glutSetCursor(GLUT_CURSOR_HELP);
      break;
    case CursorType::COLLIDER:
      glutSetCursor(GLUT_CURSOR_CROSSHAIR);
      break;
    case CursorType::SELECT:
      glutSetCursor(GLUT_CURSOR_INHERIT);
      break;
    case CursorType::ROTATE:
      glutSetCursor(GLUT_CURSOR_CYCLE);
      break;
    default:
      log_debug("GlutDisplay: Unhandled cursor type: {}", static_cast<int>(cursor));
      break;
  }
}

} // namespace construo

/* EOF */
