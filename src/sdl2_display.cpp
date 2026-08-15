// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sdl2_display.hpp"

#include <stdexcept>

#include <logmich/log.hpp>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

#include "action.hpp"
#include "controller.hpp"
#include "events.hpp"
#include "screen_manager.hpp"
#include "settings.hpp"
#include "cursors/cursors.hpp"

namespace construo {

namespace {

void init_default_keybindings(SDL2Display& dpy)
{
  dpy.bind_key(SDLK_DELETE, Action::DELETE);
  dpy.bind_key(SDLK_BACKSPACE, Action::DELETE);
  dpy.bind_key(SDLK_SPACE, Action::RUN);
  dpy.bind_key(SDLK_TAB, Action::TOGGLESLOWMO);
  dpy.bind_key(SDLK_ESCAPE, Action::ESCAPE);
  dpy.bind_key(SDLK_q, Action::ESCAPE);
  dpy.bind_key(SDLK_h, Action::FLIP);
  dpy.bind_key(SDLK_f, Action::FIX);
  dpy.bind_key(SDLK_d, Action::DUPLICATE);
  dpy.bind_key(SDLK_v, Action::SETVELOCITY);
  dpy.bind_key(SDLK_c, Action::CLEAR);
  dpy.bind_key(SDLK_j, Action::JOIN);
  dpy.bind_key(SDLK_s, Action::SCALE);
  dpy.bind_key(SDLK_a, Action::ACTIONCAM);
  dpy.bind_key(SDLK_o, Action::HIDEDOTS);
  dpy.bind_key(SDLK_g, Action::GRID);
  dpy.bind_key(SDLK_u, Action::UNDO);
  dpy.bind_key(SDLK_r, Action::REDO);
  dpy.bind_key(SDLK_PLUS, Action::ZOOM_IN);
  dpy.bind_key(SDLK_EQUALS, Action::ZOOM_IN);
  dpy.bind_key(SDLK_MINUS, Action::ZOOM_OUT);
  dpy.bind_key(SDLK_F11, Action::FULLSCREEN);
  dpy.bind_key(SDLK_LEFT, Action::SCROLL_LEFT);
  dpy.bind_key(SDLK_RIGHT, Action::SCROLL_RIGHT);
  dpy.bind_key(SDLK_UP, Action::SCROLL_UP);
  dpy.bind_key(SDLK_DOWN, Action::SCROLL_DOWN);
  dpy.bind_key(SDLK_0, Action::QUICKSAVE0);
  dpy.bind_key(SDLK_1, Action::QUICKSAVE1);
  dpy.bind_key(SDLK_2, Action::QUICKSAVE2);
  dpy.bind_key(SDLK_3, Action::QUICKSAVE3);
  dpy.bind_key(SDLK_4, Action::QUICKSAVE4);
  dpy.bind_key(SDLK_5, Action::QUICKSAVE5);
  dpy.bind_key(SDLK_6, Action::QUICKSAVE6);
  dpy.bind_key(SDLK_7, Action::QUICKSAVE7);
  dpy.bind_key(SDLK_8, Action::QUICKSAVE8);
  dpy.bind_key(SDLK_9, Action::QUICKSAVE9);
}

} // namespace

SDL2Display::SDL2Display(std::string const& title, int width, int height, bool fullscreen) :
  m_window(nullptr),
  m_gl(nullptr),
  m_renderer(),
  m_size(width, height),
  m_mouse_pos(),
  m_is_fullscreen(fullscreen),
  m_title(title),
  m_key_bindings(),
  m_cursor_select(nullptr),
  m_cursor_scroll(nullptr),
  m_cursor_zoom(nullptr),
  m_cursor_insert(nullptr),
  m_cursor_collider(nullptr),
  m_active_cursor(nullptr)
{
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
    throw std::runtime_error(std::string("SDL_Init failed: ") + SDL_GetError());
  }

  auto set_common_gl_attrs = []() {
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
  };

  // Prefer OpenGL ES 2.0 (WASM / Android / R36S / desktop GLES).
  // On desktop hosts without an ES driver, fall back to a compatibility
  // GL 2.1 context — shaders remain GLES2-style and work under that profile.
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  set_common_gl_attrs();

  Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  if (fullscreen) {
    flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
  }

  m_window = SDL_CreateWindow(title.c_str(),
                              SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              width, height, flags);
  if (!m_window) {
    SDL_Quit();
    throw std::runtime_error(std::string("SDL_CreateWindow failed: ") + SDL_GetError());
  }

  m_gl = SDL_GL_CreateContext(m_window);
  if (!m_gl) {
    log_info("GLES2 context failed ({}), trying desktop GL 2.1 compatibility", SDL_GetError());
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    set_common_gl_attrs();
    m_gl = SDL_GL_CreateContext(m_window);
  }
  if (!m_gl) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
    SDL_Quit();
    throw std::runtime_error(std::string("SDL_GL_CreateContext failed: ") + SDL_GetError());
  }

  SDL_GL_MakeCurrent(m_window, m_gl);
  SDL_GL_SetSwapInterval(1);

  int drawable_w = 0, drawable_h = 0;
  SDL_GL_GetDrawableSize(m_window, &drawable_w, &drawable_h);
  m_size = geom::isize(drawable_w, drawable_h);

  m_renderer.init();
  m_renderer.set_viewport(m_size);

  init_default_keybindings(*this);
  load_cursors();
  set_cursor(CursorType::INSERT);
  log_info("SDL2Display ready ({}x{}, GLES2)", m_size.width(), m_size.height());
}

SDL2Display::~SDL2Display()
{
  free_cursors();
  m_renderer.shutdown();
  if (m_gl) {
    SDL_GL_DeleteContext(m_gl);
    m_gl = nullptr;
  }
  if (m_window) {
    SDL_DestroyWindow(m_window);
    m_window = nullptr;
  }
  SDL_Quit();
}

void
SDL2Display::bind_key(SDL_Keycode key, Action action)
{
  m_key_bindings[key] = action;
}

geom::frect
SDL2Display::geometry() const
{
  return geom::frect(geom::fsize(m_size));
}

geom::fpoint
SDL2Display::get_mouse_pos() const
{
  return geom::fpoint(static_cast<float>(m_mouse_pos.x()),
                      static_cast<float>(m_mouse_pos.y()));
}

void
SDL2Display::clear()
{
  m_renderer.clear(Color(0.0f, 0.0f, 0.0f, 1.0f));
}

void
SDL2Display::flip()
{
  SDL_GL_SwapWindow(m_window);
}

void
SDL2Display::draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide)
{
  m_renderer.draw_line(p1, p2, color, wide > 0 ? static_cast<float>(wide) : 1.0f);
}

void
SDL2Display::draw_lines(std::vector<Line>& lines, Color color, int wide)
{
  m_renderer.draw_lines(lines, color, wide > 0 ? static_cast<float>(wide) : 1.0f);
}

void
SDL2Display::draw_rect(geom::frect const& rect, Color color)
{
  m_renderer.draw_rect(rect, color);
}

void
SDL2Display::draw_fill_rect(geom::frect const& rect, Color color)
{
  m_renderer.draw_fill_rect(rect, color);
}

void
SDL2Display::draw_circle(geom::fpoint const& pos, float r, Color color)
{
  m_renderer.draw_circle(pos, r, color);
}

void
SDL2Display::draw_circles(std::vector<Circle>& circles, Color color)
{
  m_renderer.draw_circles(circles, color);
}

void
SDL2Display::draw_fill_circle(geom::fpoint const& pos, float r, Color color)
{
  m_renderer.draw_fill_circle(pos, r, color);
}

void
SDL2Display::draw_string(geom::fpoint const& pos, const std::string& str, Color color)
{
  m_renderer.draw_string(pos, str, color);
}

void
SDL2Display::draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color)
{
  m_renderer.draw_string_centered(pos, str, color);
}

void
SDL2Display::set_clip_rect(geom::frect const& rect)
{
  m_renderer.set_clip_rect(rect);
}

void
SDL2Display::clear_clip_rect()
{
  m_renderer.clear_clip_rect();
}

void
SDL2Display::push_quick_draw()
{
  m_renderer.push_quick_draw();
}

void
SDL2Display::pop_quick_draw()
{
  m_renderer.pop_quick_draw();
}

SDL_Cursor*
SDL2Display::make_xbm_cursor(unsigned char const* bits,
                             unsigned char const* mask_bits,
                             int width, int height,
                             int hot_x, int hot_y)
{
  // SDL_CreateCursor expects MSB-first bitmaps (same layout as XBM).
  SDL_Cursor* c = SDL_CreateCursor(bits, mask_bits, width, height, hot_x, hot_y);
  if (!c) {
    log_warn("SDL_CreateCursor failed: {}", SDL_GetError());
  }
  return c;
}

void
SDL2Display::load_cursors()
{
  m_cursor_select = make_xbm_cursor(
    cursor_select_bits, cursor_select_mask_bits,
    cursor_select_width, cursor_select_height,
    cursor_select_x_hot, cursor_select_y_hot);
  m_cursor_scroll = make_xbm_cursor(
    cursor_scroll_bits, cursor_scroll_mask_bits,
    cursor_scroll_width, cursor_scroll_height,
    cursor_scroll_x_hot, cursor_scroll_y_hot);
  m_cursor_zoom = make_xbm_cursor(
    cursor_zoom_bits, cursor_zoom_mask_bits,
    cursor_zoom_width, cursor_zoom_height,
    cursor_zoom_x_hot, cursor_zoom_y_hot);
  m_cursor_insert = make_xbm_cursor(
    cursor_insert_bits, cursor_insert_mask_bits,
    cursor_insert_width, cursor_insert_height,
    cursor_insert_x_hot, cursor_insert_y_hot);
  m_cursor_collider = make_xbm_cursor(
    cursor_collider_bits, cursor_collider_mask_bits,
    cursor_collider_width, cursor_collider_height,
    cursor_collider_x_hot, cursor_collider_y_hot);
}

void
SDL2Display::free_cursors()
{
  // Do not free the active cursor while it is still set.
  SDL_SetCursor(SDL_GetDefaultCursor());
  m_active_cursor = nullptr;
  auto free_one = [](SDL_Cursor*& c) {
    if (c) {
      SDL_FreeCursor(c);
      c = nullptr;
    }
  };
  free_one(m_cursor_select);
  free_one(m_cursor_scroll);
  free_one(m_cursor_zoom);
  free_one(m_cursor_insert);
  free_one(m_cursor_collider);
}

void
SDL2Display::set_cursor_real(CursorType cursor)
{
  SDL_Cursor* c = nullptr;
  switch (cursor) {
    case CursorType::SELECT:
      c = m_cursor_select;
      break;
    case CursorType::SCROLL:
      c = m_cursor_scroll;
      break;
    case CursorType::ZOOM:
      c = m_cursor_zoom;
      break;
    case CursorType::INSERT:
      c = m_cursor_insert;
      break;
    case CursorType::COLLIDER:
      c = m_cursor_collider;
      break;
    case CursorType::ROTATE:
    case CursorType::SCALE:
      c = m_cursor_scroll;
      break;
  }
  if (c) {
    SDL_SetCursor(c);
    m_active_cursor = c;
  }
}

void
SDL2Display::enter_fullscreen()
{
  if (!m_is_fullscreen) {
    SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    m_is_fullscreen = true;
    int w = 0, h = 0;
    SDL_GL_GetDrawableSize(m_window, &w, &h);
    m_size = geom::isize(w, h);
    m_renderer.set_viewport(m_size);
    if (ScreenManager::instance()) {
      ScreenManager::instance()->set_geometry(geometry());
    }
  }
}

void
SDL2Display::leave_fullscreen()
{
  if (m_is_fullscreen) {
    SDL_SetWindowFullscreen(m_window, 0);
    m_is_fullscreen = false;
    int w = 0, h = 0;
    SDL_GL_GetDrawableSize(m_window, &w, &h);
    m_size = geom::isize(w, h);
    m_renderer.set_viewport(m_size);
    if (ScreenManager::instance()) {
      ScreenManager::instance()->set_geometry(geometry());
    }
  }
}

void
SDL2Display::toggle_fullscreen()
{
  if (m_is_fullscreen) {
    leave_fullscreen();
  } else {
    enter_fullscreen();
  }
}

void
SDL2Display::handle_key(SDL_Keycode key, bool pressed)
{
  auto it = m_key_bindings.find(key);
  if (it == m_key_bindings.end()) {
    return;
  }
  Event ev;
  ev.button.type = BUTTON_EVENT;
  ev.button.id = it->second;
  ev.button.pressed = pressed;
  events.push(ev);
}


geom::ipoint
SDL2Display::window_to_drawable(int x, int y) const
{
  int win_w = 0, win_h = 0;
  SDL_GetWindowSize(m_window, &win_w, &win_h);
  if (win_w <= 0 || win_h <= 0) {
    return geom::ipoint(x, y);
  }
  // m_size is the GL drawable size (may differ under HiDPI).
  float sx = static_cast<float>(m_size.width()) / static_cast<float>(win_w);
  float sy = static_cast<float>(m_size.height()) / static_cast<float>(win_h);
  return geom::ipoint(static_cast<int>(static_cast<float>(x) * sx + 0.5f),
                      static_cast<int>(static_cast<float>(y) * sy + 0.5f));
}

void
SDL2Display::process_event(SDL_Event const& ev)
{
  switch (ev.type) {
    case SDL_QUIT: {
      Event e;
      e.button.type = BUTTON_EVENT;
      e.button.id = Action::ESCAPE;
      e.button.pressed = true;
      events.push(e);
      break;
    }
    case SDL_WINDOWEVENT:
      if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED ||
          ev.window.event == SDL_WINDOWEVENT_RESIZED) {
        int w = 0, h = 0;
        SDL_GL_GetDrawableSize(m_window, &w, &h);
        m_size = geom::isize(w, h);
        m_renderer.set_viewport(m_size);
        if (ScreenManager::instance()) {
          ScreenManager::instance()->set_geometry(geometry());
        }
      }
      break;
    case SDL_KEYDOWN:
      handle_key(ev.key.keysym.sym, true);
      break;
    case SDL_KEYUP:
      handle_key(ev.key.keysym.sym, false);
      break;
    case SDL_MOUSEMOTION:
      m_mouse_pos = window_to_drawable(ev.motion.x, ev.motion.y);
      break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP: {
      m_mouse_pos = window_to_drawable(ev.button.x, ev.button.y);
      Action id = Action::NONE;
      if (ev.button.button == SDL_BUTTON_LEFT) {
        id = Action::PRIMARY;
      } else if (ev.button.button == SDL_BUTTON_RIGHT) {
        id = Action::SECONDARY;
      } else if (ev.button.button == SDL_BUTTON_MIDDLE) {
        id = Action::TERTIARY;
      }
      if (id != Action::NONE) {
        Event e;
        e.button.type = BUTTON_EVENT;
        e.button.id = id;
        e.button.pressed = (ev.type == SDL_MOUSEBUTTONDOWN);
        events.push(e);
      }
      break;
    }
    case SDL_MOUSEWHEEL: {
      Event e;
      e.button.type = BUTTON_EVENT;
      e.button.id = (ev.wheel.y > 0) ? Action::ZOOM_IN : Action::ZOOM_OUT;
      e.button.pressed = true;
      events.push(e);
      break;
    }
    case SDL_FINGERDOWN:
    case SDL_FINGERUP:
    case SDL_FINGERMOTION: {
      // Normalized 0..1 → window pixels
      m_mouse_pos = geom::ipoint(
        static_cast<int>(ev.tfinger.x * static_cast<float>(m_size.width())),
        static_cast<int>(ev.tfinger.y * static_cast<float>(m_size.height())));
      if (ev.type == SDL_FINGERDOWN || ev.type == SDL_FINGERUP) {
        Event e;
        e.button.type = BUTTON_EVENT;
        // One finger = primary; two-finger contact tracked roughly via fingerId
        e.button.id = (ev.tfinger.fingerId == 0) ? Action::PRIMARY : Action::SECONDARY;
        e.button.pressed = (ev.type == SDL_FINGERDOWN);
        events.push(e);
      }
      break;
    }
    default:
      break;
  }
}

namespace {

#ifdef __EMSCRIPTEN__
SDL2Display* g_em_display = nullptr;

void em_main_loop_callback()
{
  if (!g_em_display) {
    return;
  }
  if (ScreenManager::instance()->is_finished()) {
    emscripten_cancel_main_loop();
    return;
  }
  SDL_Event ev;
  while (SDL_PollEvent(&ev)) {
    g_em_display->process_event_public(ev);
  }
  ScreenManager::instance()->run_once(*g_em_display);
}
#endif

} // namespace

void
SDL2Display::process_event_public(SDL_Event const& ev)
{
  process_event(ev);
}

void
SDL2Display::run()
{
#ifdef __EMSCRIPTEN__
  g_em_display = this;
  // fps=0 → browser refresh rate; simulate_infinite_loop=1
  emscripten_set_main_loop(em_main_loop_callback, 0, 1);
#else
  while (!ScreenManager::instance()->is_finished()) {
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
      process_event(ev);
    }
    ScreenManager::instance()->run_once(*this);
  }
#endif
}

} // namespace construo

/* EOF */
