// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sdl2_display.hpp"

#if defined(__ANDROID__)
#  include <android/log.h>
#  define CONSTRUO_ALOG(...) __android_log_print(ANDROID_LOG_INFO, "construo", __VA_ARGS__)
#else
#  define CONSTRUO_ALOG(...) do { } while (0)
#endif

#include <stdexcept>
#include <vector>

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
  // Shift+digit quickload (GLUT uses !@#$…); also F1–F10 for load without shift.
  dpy.bind_key(SDLK_F1, Action::QUICKLOAD1);
  dpy.bind_key(SDLK_F2, Action::QUICKLOAD2);
  dpy.bind_key(SDLK_F3, Action::QUICKLOAD3);
  dpy.bind_key(SDLK_F4, Action::QUICKLOAD4);
  dpy.bind_key(SDLK_F5, Action::QUICKLOAD5);
  dpy.bind_key(SDLK_F6, Action::QUICKLOAD6);
  dpy.bind_key(SDLK_F7, Action::QUICKLOAD7);
  dpy.bind_key(SDLK_F8, Action::QUICKLOAD8);
  dpy.bind_key(SDLK_F9, Action::QUICKLOAD9);
  dpy.bind_key(SDLK_F10, Action::QUICKLOAD0);
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
  m_active_cursor(nullptr),
  m_controller(nullptr)
{
  // Platform hints before SDL_Init (Android / Emscripten / high-DPI).
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
  SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
#ifdef __ANDROID__
  SDL_SetHint(SDL_HINT_ORIENTATIONS, "LandscapeLeft LandscapeRight");
#endif
#ifdef __EMSCRIPTEN__
  SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");
#endif

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0) {
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
  // Prefer adaptive vsync; fall back to fixed vsync.
  if (SDL_GL_SetSwapInterval(-1) != 0) {
    SDL_GL_SetSwapInterval(1);
  }

  int drawable_w = 0, drawable_h = 0;
  SDL_GL_GetDrawableSize(m_window, &drawable_w, &drawable_h);
  m_size = geom::isize(drawable_w, drawable_h);

  m_renderer.init();
  m_renderer.set_viewport(m_size);

  init_default_keybindings(*this);
  load_cursors();

  // Window / taskbar icon (BMP next to data or beside executable).
  {
    char const* candidates[] = {
      "data/icons/construo-window.bmp",
      "construo-window.bmp",
      nullptr
    };
    for (int i = 0; candidates[i]; ++i) {
      SDL_Surface* icon = SDL_LoadBMP(candidates[i]);
      if (icon) {
        SDL_SetWindowIcon(m_window, icon);
        SDL_FreeSurface(icon);
        break;
      }
    }
  }

  open_controller();
  set_cursor(CursorType::INSERT);
  log_info("SDL2Display ready ({}x{}, GLES2)", m_size.width(), m_size.height());
  CONSTRUO_ALOG("SDL2Display ready %dx%d", m_size.width(), m_size.height());
}

SDL2Display::~SDL2Display()
{
  close_controller();
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
  // XBM is LSB-first (X11); SDL_CreateCursor is MSB-first. Bit-reverse each
  // byte so the hotspot and glyph match the X11 cursors.
  auto const nbytes = static_cast<size_t>(((width + 7) / 8) * height);
  std::vector<unsigned char> data(nbytes), mask(nbytes);
  auto rev = [](unsigned char b) -> unsigned char {
    b = static_cast<unsigned char>((b & 0xF0) >> 4 | (b & 0x0F) << 4);
    b = static_cast<unsigned char>((b & 0xCC) >> 2 | (b & 0x33) << 2);
    b = static_cast<unsigned char>((b & 0xAA) >> 1 | (b & 0x55) << 1);
    return b;
  };
  for (size_t i = 0; i < nbytes; ++i) {
    data[i] = rev(bits[i]);
    mask[i] = rev(mask_bits[i]);
  }
  SDL_Cursor* c = SDL_CreateCursor(data.data(), mask.data(), width, height, hot_x, hot_y);
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
  Action action = Action::NONE;

  // Shift+0–9 → quickload (matches GLUT shifted-symbol bindings).
  SDL_Keymod const mods = SDL_GetModState();
  bool const shift = (mods & KMOD_SHIFT) != 0;
  if (shift) {
    switch (key) {
      case SDLK_0: action = Action::QUICKLOAD0; break;
      case SDLK_1: action = Action::QUICKLOAD1; break;
      case SDLK_2: action = Action::QUICKLOAD2; break;
      case SDLK_3: action = Action::QUICKLOAD3; break;
      case SDLK_4: action = Action::QUICKLOAD4; break;
      case SDLK_5: action = Action::QUICKLOAD5; break;
      case SDLK_6: action = Action::QUICKLOAD6; break;
      case SDLK_7: action = Action::QUICKLOAD7; break;
      case SDLK_8: action = Action::QUICKLOAD8; break;
      case SDLK_9: action = Action::QUICKLOAD9; break;
      default: break;
    }
  }

  if (action == Action::NONE) {
    auto it = m_key_bindings.find(key);
    if (it == m_key_bindings.end()) {
      return;
    }
    action = it->second;
  }

  Event ev;
  ev.button.type = BUTTON_EVENT;
  ev.button.id = action;
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
SDL2Display::open_controller()
{
  int const n = SDL_NumJoysticks();
  for (int i = 0; i < n; ++i) {
    if (!SDL_IsGameController(i)) {
      continue;
    }
    m_controller = SDL_GameControllerOpen(i);
    if (m_controller) {
      log_info("Opened game controller: {}", SDL_GameControllerName(m_controller));
      return;
    }
  }
}

void
SDL2Display::close_controller()
{
  if (m_controller) {
    SDL_GameControllerClose(m_controller);
    m_controller = nullptr;
  }
}

void
SDL2Display::emit_button(Action action, bool pressed)
{
  Event e;
  e.button.type = BUTTON_EVENT;
  e.button.id = action;
  e.button.pressed = pressed;
  events.push(e);
}

void
SDL2Display::handle_controller_button(SDL_GameControllerButton button, bool pressed)
{
  Action action = Action::NONE;
  switch (button) {
    case SDL_CONTROLLER_BUTTON_A: action = Action::PRIMARY; break;
    case SDL_CONTROLLER_BUTTON_B: action = Action::SECONDARY; break;
    case SDL_CONTROLLER_BUTTON_X: action = Action::FIX; break;
    case SDL_CONTROLLER_BUTTON_Y: action = Action::RUN; break;
    case SDL_CONTROLLER_BUTTON_START: action = Action::ESCAPE; break;
    case SDL_CONTROLLER_BUTTON_BACK: action = Action::UNDO; break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: action = Action::ZOOM_OUT; break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: action = Action::ZOOM_IN; break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: action = Action::SCROLL_UP; break;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: action = Action::SCROLL_DOWN; break;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: action = Action::SCROLL_LEFT; break;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: action = Action::SCROLL_RIGHT; break;
    default: return;
  }
  emit_button(action, pressed);
}

void
SDL2Display::handle_controller_axis()
{
  if (!m_controller) {
    return;
  }
  // Left stick → continuous scroll (edge-triggered on threshold cross).
  constexpr Sint16 dead = 16000;
  struct AxisMap { SDL_GameControllerAxis axis; Action neg; Action pos; bool* state_neg; bool* state_pos; };
  static bool left=false, right=false, up=false, down=false;
  static bool zin=false, zout=false, ltrig=false, rtrig=false;
  AxisMap maps[] = {
    { SDL_CONTROLLER_AXIS_LEFTX, Action::SCROLL_LEFT, Action::SCROLL_RIGHT, &left, &right },
    { SDL_CONTROLLER_AXIS_LEFTY, Action::SCROLL_UP, Action::SCROLL_DOWN, &up, &down },
    { SDL_CONTROLLER_AXIS_RIGHTY, Action::ZOOM_IN, Action::ZOOM_OUT, &zin, &zout },
  };
  // Triggers are 0..32767 (never negative). Treat as "pos" only.
  {
    Sint16 lt = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    Sint16 rt = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    bool l = lt > dead;
    bool r = rt > dead;
    if (l != ltrig) { emit_button(Action::UNDO, l); ltrig = l; }
    if (r != rtrig) { emit_button(Action::REDO, r); rtrig = r; }
  }
  for (auto& m : maps) {
    Sint16 v = SDL_GameControllerGetAxis(m_controller, m.axis);
    bool neg = v < -dead;
    bool pos = v > dead;
    if (neg != *m.state_neg) {
      emit_button(m.neg, neg);
      *m.state_neg = neg;
    }
    if (pos != *m.state_pos) {
      emit_button(m.pos, pos);
      *m.state_pos = pos;
    }
  }
}

void
SDL2Display::process_event(SDL_Event const& ev)
{
  switch (ev.type) {
    case SDL_CONTROLLERDEVICEADDED:
      if (!m_controller) {
        open_controller();
      }
      break;
    case SDL_CONTROLLERDEVICEREMOVED:
      close_controller();
      break;
    case SDL_CONTROLLERBUTTONDOWN:
      handle_controller_button(
        static_cast<SDL_GameControllerButton>(ev.cbutton.button), true);
      break;
    case SDL_CONTROLLERBUTTONUP:
      handle_controller_button(
        static_cast<SDL_GameControllerButton>(ev.cbutton.button), false);
      break;
    case SDL_QUIT:
      CONSTRUO_ALOG("SDL_QUIT received"); {
      Event e;
      e.button.type = BUTTON_EVENT;
      e.button.id = Action::ESCAPE;
      e.button.pressed = true;
      events.push(e);
      break;
    }
#if defined(SDL_APP_TERMINATING)
    // Mobile / web lifecycle (SDL 2.0.4+). Treat hard terminate like Quit.
    case SDL_APP_TERMINATING: {
      Event e;
      e.button.type = BUTTON_EVENT;
      e.button.id = Action::ESCAPE;
      e.button.pressed = true;
      events.push(e);
      break;
    }
#endif
#if defined(SDL_APP_WILLENTERBACKGROUND)
    case SDL_APP_WILLENTERBACKGROUND:
      // Pause simulation while the activity is not visible.
      if (Controller::instance() && Controller::instance()->is_running()) {
        Event e;
        e.button.type = BUTTON_EVENT;
        e.button.id = Action::RUN;
        e.button.pressed = true;
        events.push(e);
      }
      break;
#endif
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
      if (ev.key.repeat) {
        break;
      }
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
  g_em_display->poll_controller_axes();
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
  CONSTRUO_ALOG("SDL2Display::run()");
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
    handle_controller_axis();
    ScreenManager::instance()->run_once(*this);
  }
#endif
}

} // namespace construo

/* EOF */
