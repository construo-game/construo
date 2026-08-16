// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "sdl2_display.hpp"

#if defined(__ANDROID__)
#  include <android/log.h>
#  define CONSTRUO_ALOG(...) __android_log_print(ANDROID_LOG_INFO, "construo", __VA_ARGS__)
#else
#  define CONSTRUO_ALOG(...) do { } while (0)
#endif

#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include <logmich/log.hpp>

#ifdef __EMSCRIPTEN__
#  include <emscripten.h>
#endif

#include "action.hpp"
#include "controller.hpp"
#include "events.hpp"
#include "screen_manager.hpp"
#include "worldview_widget.hpp"
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
    std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    std::fflush(stderr);
    std::exit(EXIT_FAILURE);
  }

  // Log video backend early — KMSDRM vs x11/wayland matters for EGL/GLES on
  // handhelds (R36S/ArkOS, Mali). Printed to stderr so it shows even when
  // C++ exception unwinding is unreliable with the ArkOS/cxxabi shim.
  {
    char const* driver = SDL_GetCurrentVideoDriver();
    std::fprintf(stderr, "SDL video driver: env=%s current=%s\n",
                 std::getenv("SDL_VIDEODRIVER") ? std::getenv("SDL_VIDEODRIVER") : "(unset)",
                 driver ? driver : "(none)");
    int n = SDL_GetNumVideoDrivers();
    std::fprintf(stderr, "SDL available video drivers:");
    for (int i = 0; i < n; ++i) {
      std::fprintf(stderr, " %s", SDL_GetVideoDriver(i));
    }
    std::fprintf(stderr, "\n");
    std::fflush(stderr);
  }

  // Probe libEGL: KMSDRM GLES needs it at runtime (SDL dlopens it).
  {
    void* egl = SDL_LoadObject("libEGL.so.1");
    if (!egl) egl = SDL_LoadObject("libEGL.so");
    if (!egl) egl = SDL_LoadObject("/usr/lib/aarch64-linux-gnu/libEGL.so.1");
    if (!egl) egl = SDL_LoadObject("/usr/local/lib/aarch64-linux-gnu/libEGL.so.1");
    if (egl) {
      std::fprintf(stderr, "libEGL probe: loaded\n");
      SDL_UnloadObject(egl);
    } else {
      std::fprintf(stderr, "libEGL probe: FAILED (%s) — matching libEGL next to libGLESv2 may be required\n",
                   SDL_GetError());
    }
    std::fflush(stderr);
  }

  // Leave pixel format / buffer sizes on SDL defaults ("auto"). Explicit
  // RED/GREEN/BLUE/ALPHA sizes caused bad GBM BOs and SIGBUS in gbm_bo_write
  // on Mali KMSDRM (R36S).
  //
  // Windows: request desktop OpenGL 3.3 core first. GLES/ANGLE is often
  // missing and SDL then fails CreateWindow with "Could not initialize
  // OpenGL / GLES library". Embedded (Android / KMSDRM / Emscripten) keep ES2.
  SDL_GL_ResetAttributes();
#if defined(_WIN32) || defined(WIN32)
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  char const* video_drv = SDL_GetCurrentVideoDriver();
  bool const is_kmsdrm = video_drv &&
    (std::strcmp(video_drv, "KMSDRM") == 0 || std::strcmp(video_drv, "kmsdrm") == 0);

  // Prefer native display size when fullscreen / on KMSDRM — exclusive modes
  // that do not match the panel (common on R36S 640x480 + KMSDRM) often fail
  // with EGL surface errors. On pure KMSDRM there is no real "windowed" mode.
  int req_w = width;
  int req_h = height;
  if (fullscreen || is_kmsdrm) {
    SDL_DisplayMode mode;
    if (SDL_GetDesktopDisplayMode(0, &mode) == 0 && mode.w > 0 && mode.h > 0) {
      std::fprintf(stderr, "desktop mode %dx%d @%dHz\n", mode.w, mode.h, mode.refresh_rate);
      if (req_w != mode.w || req_h != mode.h) {
        std::fprintf(stderr, "adjusting requested %dx%d -> desktop %dx%d\n",
                     req_w, req_h, mode.w, mode.h);
        req_w = mode.w;
        req_h = mode.h;
      }
    } else {
      std::fprintf(stderr, "SDL_GetDesktopDisplayMode failed: %s\n", SDL_GetError());
    }
    std::fflush(stderr);
  }

  // Attempt order on embedded GLES (ArkOS/R36S, Mali, KMSDRM):
  // 1) FULLSCREEN_DESKTOP  2) exclusive FULLSCREEN  3) windowed
  // On KMSDRM always try fullscreen first even if the user omitted -f —
  // windowed EGL surfaces are a common source of gbm_bo_write SIGBUS.
  struct Attempt {
    char const* name;
    Uint32 flags;
  };
  Attempt attempts[3];
  int n_attempts = 0;
#ifdef __ANDROID__
  attempts[n_attempts++] = Attempt{"android-fullscreen",
                                   SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN};
  (void)fullscreen;
  (void)is_kmsdrm;
#else
  if (fullscreen || is_kmsdrm) {
    attempts[n_attempts++] = Attempt{"fullscreen-desktop",
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP};
    attempts[n_attempts++] = Attempt{"fullscreen-exclusive",
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN};
  }
  // Windowed last; drop RESIZABLE/HIGHDPI on KMSDRM (they confuse the DRM path).
  Uint32 win_flags = SDL_WINDOW_OPENGL;
  if (!is_kmsdrm) {
    win_flags |= SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
  }
  attempts[n_attempts++] = Attempt{"windowed", win_flags};
#endif

  std::string last_error;
  for (int ai = 0; ai < n_attempts && !m_gl; ++ai) {
    std::fprintf(stderr, "trying window=%s %dx%d flags=0x%x\n",
                 attempts[ai].name, req_w, req_h,
                 static_cast<unsigned>(attempts[ai].flags));
    std::fflush(stderr);
    SDL_ClearError();
    m_window = SDL_CreateWindow(title.c_str(),
                                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                req_w, req_h, attempts[ai].flags);
    if (!m_window) {
      last_error = std::string("SDL_CreateWindow(") + attempts[ai].name + "): " + SDL_GetError();
      std::fprintf(stderr, "%s\n", last_error.c_str());
      std::fflush(stderr);
      continue;
    }
    m_gl = SDL_GL_CreateContext(m_window);
    if (!m_gl) {
      last_error = std::string("SDL_GL_CreateContext(") + attempts[ai].name + "): " + SDL_GetError();
      std::fprintf(stderr, "%s\n", last_error.c_str());
      std::fflush(stderr);
      SDL_DestroyWindow(m_window);
      m_window = nullptr;
      // Fallbacks when the preferred profile failed.
      struct GlFallback { char const* name; int profile; int major; int minor; };
      GlFallback fallbacks[] = {
#if defined(_WIN32) || defined(WIN32)
        {"gl33-compat", SDL_GL_CONTEXT_PROFILE_COMPATIBILITY, 3, 3},
        {"gl21-compat", SDL_GL_CONTEXT_PROFILE_COMPATIBILITY, 2, 1},
        {"es2", SDL_GL_CONTEXT_PROFILE_ES, 2, 0},
#else
        {"gl21-compat", SDL_GL_CONTEXT_PROFILE_COMPATIBILITY, 2, 1},
        {"gl33-core", SDL_GL_CONTEXT_PROFILE_CORE, 3, 3},
#endif
      };
      for (auto const& fb : fallbacks) {
        SDL_GL_ResetAttributes();
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, fb.profile);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, fb.major);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, fb.minor);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        m_window = SDL_CreateWindow(title.c_str(),
                                    SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                    req_w, req_h, attempts[ai].flags);
        if (!m_window) {
          last_error = std::string("SDL_CreateWindow(") + attempts[ai].name + "/" + fb.name + "): " + SDL_GetError();
          std::fprintf(stderr, "%s\n", last_error.c_str());
          std::fflush(stderr);
          continue;
        }
        m_gl = SDL_GL_CreateContext(m_window);
        if (!m_gl) {
          last_error = std::string("SDL_GL_CreateContext(") + fb.name + "): " + SDL_GetError();
          std::fprintf(stderr, "%s\n", last_error.c_str());
          std::fflush(stderr);
          SDL_DestroyWindow(m_window);
          m_window = nullptr;
          continue;
        }
        std::fprintf(stderr, "created window+context with %s / %s\n", attempts[ai].name, fb.name);
        std::fflush(stderr);
        break;
      }
      // Restore preferred profile for the next window-flag attempt.
      SDL_GL_ResetAttributes();
#if defined(_WIN32) || defined(WIN32)
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
#else
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif
      SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
      continue;
    }
    std::fprintf(stderr, "created window+context with %s / es2-auto\n", attempts[ai].name);
    std::fflush(stderr);
    m_is_fullscreen = (attempts[ai].flags & (SDL_WINDOW_FULLSCREEN | SDL_WINDOW_FULLSCREEN_DESKTOP)) != 0;
  }

  if (!m_window || !m_gl) {
    if (m_window) {
      SDL_DestroyWindow(m_window);
      m_window = nullptr;
    }
    SDL_Quit();
    std::fprintf(stderr, "SDL2Display: failed to create window/GL context: %s\n",
                 last_error.empty() ? "(unknown)" : last_error.c_str());
    std::fflush(stderr);
    // Avoid C++ throw here: ArkOS + GCC15 libgcc_eh/cxxabi shim often aborts
    // in _Unwind_Resume instead of delivering the exception to the catch in
    // ConstruoMain::run. Print and exit so the message is always visible.
    std::exit(EXIT_FAILURE);
  }

  std::fprintf(stderr, "debug: before SDL_GL_MakeCurrent\n");
  std::fflush(stderr);
  if (SDL_GL_MakeCurrent(m_window, m_gl) != 0) {
    std::fprintf(stderr, "SDL_GL_MakeCurrent failed: %s\n", SDL_GetError());
    std::fflush(stderr);
    std::exit(EXIT_FAILURE);
  }
  std::fprintf(stderr, "debug: after SDL_GL_MakeCurrent\n");
  std::fflush(stderr);

  // Prefer adaptive vsync; fall back to fixed vsync. On some Mali builds
  // adaptive (-1) misbehaves — try fixed first on KMSDRM.
  std::fprintf(stderr, "debug: before SDL_GL_SetSwapInterval\n");
  std::fflush(stderr);
  if (is_kmsdrm) {
    if (SDL_GL_SetSwapInterval(1) != 0) {
      std::fprintf(stderr, "debug: SetSwapInterval(1) failed: %s, trying 0\n", SDL_GetError());
      std::fflush(stderr);
      SDL_GL_SetSwapInterval(0);
    }
  } else if (SDL_GL_SetSwapInterval(-1) != 0) {
    SDL_GL_SetSwapInterval(1);
  }
  std::fprintf(stderr, "debug: after SDL_GL_SetSwapInterval (kmsdrm=%d)\n", is_kmsdrm ? 1 : 0);
  std::fflush(stderr);

  int drawable_w = 0, drawable_h = 0;
  SDL_GL_GetDrawableSize(m_window, &drawable_w, &drawable_h);
  if (drawable_w <= 0 || drawable_h <= 0) {
    SDL_GetWindowSize(m_window, &drawable_w, &drawable_h);
  }
  m_size = geom::isize(drawable_w, drawable_h);
  std::fprintf(stderr, "drawable size %dx%d\n", drawable_w, drawable_h);
  std::fflush(stderr);
  m_mouse_pos = geom::ipoint(drawable_w / 2, drawable_h / 2);
  m_axis_last_ticks = SDL_GetTicks();

  std::fprintf(stderr, "debug: before m_renderer.init()\n");
  std::fflush(stderr);
  m_renderer.init();
  std::fprintf(stderr, "debug: after m_renderer.init()\n");
  std::fflush(stderr);

  std::fprintf(stderr, "debug: before set_viewport\n");
  std::fflush(stderr);
  m_renderer.set_viewport(m_size);
  std::fprintf(stderr, "debug: after set_viewport\n");
  std::fflush(stderr);

  std::fprintf(stderr, "debug: before keybindings\n");
  std::fflush(stderr);
  init_default_keybindings(*this);
  std::fprintf(stderr, "debug: after keybindings\n");
  std::fflush(stderr);

  std::fprintf(stderr, "debug: before load_cursors\n");
  std::fflush(stderr);
  load_cursors();
  std::fprintf(stderr, "debug: after load_cursors\n");
  std::fflush(stderr);

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
  std::fprintf(stderr, "debug: after window icon\n");
  std::fflush(stderr);

  std::fprintf(stderr, "debug: before open_controller\n");
  std::fflush(stderr);
  open_controller();
  std::fprintf(stderr, "debug: after open_controller\n");
  std::fflush(stderr);

  std::fprintf(stderr, "debug: before set_cursor\n");
  std::fflush(stderr);
  set_cursor(CursorType::INSERT);
  std::fprintf(stderr, "debug: after set_cursor\n");
  std::fflush(stderr);

  log_info("SDL2Display ready ({}x{}, GLES2)", m_size.width(), m_size.height());
  CONSTRUO_ALOG("SDL2Display ready %dx%d", m_size.width(), m_size.height());
  std::fprintf(stderr, "debug: SDL2Display ctor done\n");
  std::fflush(stderr);
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
  if (m_software_cursor) {
    draw_software_cursor();
  }
  SDL_GL_SwapWindow(m_window);
}

void
SDL2Display::clamp_mouse_pos()
{
  int x = m_mouse_pos.x();
  int y = m_mouse_pos.y();
  if (x < 0) x = 0;
  if (y < 0) y = 0;
  if (x >= m_size.width()) x = m_size.width() > 0 ? m_size.width() - 1 : 0;
  if (y >= m_size.height()) y = m_size.height() > 0 ? m_size.height() - 1 : 0;
  m_mouse_pos = geom::ipoint(x, y);
}

void
SDL2Display::draw_software_cursor()
{
  float const x = static_cast<float>(m_mouse_pos.x());
  float const y = static_cast<float>(m_mouse_pos.y());
  float const arm = 10.0f;
  Color const hi(1.0f, 1.0f, 1.0f, 1.0f);
  Color const lo(0.0f, 0.0f, 0.0f, 1.0f);
  // Black outline then white crosshair so it stays visible on any background.
  m_renderer.draw_line(geom::fpoint(x - arm - 1, y), geom::fpoint(x + arm + 1, y), lo, 3.0f);
  m_renderer.draw_line(geom::fpoint(x, y - arm - 1), geom::fpoint(x, y + arm + 1), lo, 3.0f);
  m_renderer.draw_line(geom::fpoint(x - arm, y), geom::fpoint(x + arm, y), hi, 1.0f);
  m_renderer.draw_line(geom::fpoint(x, y - arm), geom::fpoint(x, y + arm), hi, 1.0f);
  m_renderer.draw_fill_circle(geom::fpoint(x, y), 2.5f, hi);
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
  // SDL_CreateCursor on KMSDRM (Mali / R36S) has been observed to SIGBUS in
  // gbm_bo_write — the DRM cursor plane path is unreliable. Skip custom
  // cursors there; the default cursor or no cursor is fine for a handheld.
  char const* drv = SDL_GetCurrentVideoDriver();
  if (drv && (std::strcmp(drv, "KMSDRM") == 0 || std::strcmp(drv, "kmsdrm") == 0)) {
    std::fprintf(stderr, "debug: load_cursors: skip custom cursors on KMSDRM\n");
    std::fflush(stderr);
    SDL_ShowCursor(SDL_DISABLE);
    m_software_cursor = true;
    return;
  }

  std::fprintf(stderr, "debug: load_cursors: select\n");
  std::fflush(stderr);
  m_cursor_select = make_xbm_cursor(
    cursor_select_bits, cursor_select_mask_bits,
    cursor_select_width, cursor_select_height,
    cursor_select_x_hot, cursor_select_y_hot);
  std::fprintf(stderr, "debug: load_cursors: scroll\n");
  std::fflush(stderr);
  m_cursor_scroll = make_xbm_cursor(
    cursor_scroll_bits, cursor_scroll_mask_bits,
    cursor_scroll_width, cursor_scroll_height,
    cursor_scroll_x_hot, cursor_scroll_y_hot);
  std::fprintf(stderr, "debug: load_cursors: zoom\n");
  std::fflush(stderr);
  m_cursor_zoom = make_xbm_cursor(
    cursor_zoom_bits, cursor_zoom_mask_bits,
    cursor_zoom_width, cursor_zoom_height,
    cursor_zoom_x_hot, cursor_zoom_y_hot);
  std::fprintf(stderr, "debug: load_cursors: insert\n");
  std::fflush(stderr);
  m_cursor_insert = make_xbm_cursor(
    cursor_insert_bits, cursor_insert_mask_bits,
    cursor_insert_width, cursor_insert_height,
    cursor_insert_x_hot, cursor_insert_y_hot);
  std::fprintf(stderr, "debug: load_cursors: collider\n");
  std::fflush(stderr);
  m_cursor_collider = make_xbm_cursor(
    cursor_collider_bits, cursor_collider_mask_bits,
    cursor_collider_width, cursor_collider_height,
    cursor_collider_x_hot, cursor_collider_y_hot);
  std::fprintf(stderr, "debug: load_cursors: done\n");
  std::fflush(stderr);
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
  SDL_GameControllerEventState(SDL_ENABLE);
  int const n = SDL_NumJoysticks();
  std::fprintf(stderr, "debug: joysticks=%d\n", n);
  std::fflush(stderr);
  for (int i = 0; i < n; ++i) {
    char const* name = SDL_JoystickNameForIndex(i);
    bool const is_gc = SDL_IsGameController(i);
    std::fprintf(stderr, "debug: joy[%d] name=%s gamecontroller=%d\n",
                 i, name ? name : "?", is_gc ? 1 : 0);
    std::fflush(stderr);
    if (!is_gc) {
      continue;
    }
    m_controller = SDL_GameControllerOpen(i);
    if (m_controller) {
      log_info("Opened game controller: {}", SDL_GameControllerName(m_controller));
      std::fprintf(stderr, "debug: opened controller %s\n",
                   SDL_GameControllerName(m_controller));
      std::fflush(stderr);
      // Handhelds: stick drives the software pointer.
      if (!m_software_cursor) {
        m_software_cursor = true;
        SDL_ShowCursor(SDL_DISABLE);
      }
      return;
    }
    std::fprintf(stderr, "debug: SDL_GameControllerOpen(%d) failed: %s\n",
                 i, SDL_GetError());
    std::fflush(stderr);
  }
  // No mapped controller: still try index 0 as gamecontroller after a generic mapping
  // is not available — leave m_controller null; d-pad-only devices may appear later.
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
  // SNES/Game-Boy-style layout for handhelds (R36S):
  //   A/B = primary/secondary, X = fix, Y = hold for quick menu
  //   Start = run, Select = toggle UI chrome
  //   D-pad = tool modes, shoulders = zoom, triggers = undo/redo
  //   Stick clicks = delete / grid
  Action action = Action::NONE;
  switch (button) {
    case SDL_CONTROLLER_BUTTON_A:
      action = Action::PRIMARY;
      break;
    case SDL_CONTROLLER_BUTTON_B:
      action = Action::SECONDARY;
      break;
    case SDL_CONTROLLER_BUTTON_X:
      action = Action::FIX;
      break;
    case SDL_CONTROLLER_BUTTON_Y:
      m_y_held = pressed;
      if (!pressed) {
        m_y_menu_latched = -1;
      }
      return;
    case SDL_CONTROLLER_BUTTON_START:
      action = Action::RUN;
      break;
    case SDL_CONTROLLER_BUTTON_BACK: // Select
      action = Action::TOGGLE_UI;
      break;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
      action = Action::ZOOM_OUT;
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
      action = Action::ZOOM_IN;
      break;
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
      if (pressed && WorldViewWidget::instance()) {
        WorldViewWidget::instance()->set_mode(WorldViewWidget::INSERT_MODE);
      }
      return;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
      if (pressed && WorldViewWidget::instance()) {
        WorldViewWidget::instance()->set_mode(WorldViewWidget::SELECT_MODE);
      }
      return;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
      if (pressed && WorldViewWidget::instance()) {
        WorldViewWidget::instance()->set_mode(WorldViewWidget::COLLIDER_MODE);
      }
      return;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
      if (pressed && WorldViewWidget::instance()) {
        WorldViewWidget::instance()->set_mode(WorldViewWidget::ZOOM_MODE);
      }
      return;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK:
      action = Action::DELETE;
      break;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
      action = Action::GRID;
      break;
    default:
      return;
  }
  if (action != Action::NONE) {
    emit_button(action, pressed);
  }
}

void
SDL2Display::handle_y_quick_menu(float lx, float ly)
{
  // Cardinal sectors while Y is held (left stick). Fire once per latch.
  constexpr float dead = 0.55f;
  float const mag = std::sqrt(lx * lx + ly * ly);
  if (mag < dead) {
    m_y_menu_latched = -1;
    return;
  }
  int sector;
  if (std::fabs(ly) >= std::fabs(lx)) {
    sector = (ly < 0.0f) ? 0 : 2; // N / S
  } else {
    sector = (lx > 0.0f) ? 1 : 3; // E / W
  }
  if (sector == m_y_menu_latched) {
    return;
  }
  m_y_menu_latched = sector;
  Action action = Action::NONE;
  switch (sector) {
    case 0: action = Action::NONE; break; // Load — handled below
    case 1: action = Action::TOGGLESLOWMO; break;
    case 2: action = Action::NONE; break; // Save
    case 3: action = Action::CLEAR; break;
    default: break;
  }
  // Load/Save go through ScreenManager (not Action enum).
  if (sector == 0) {
    ScreenManager::instance()->set_gui(ScreenManager::LOAD_GUI);
    return;
  }
  if (sector == 2) {
    ScreenManager::instance()->set_gui(ScreenManager::SAVE_GUI);
    return;
  }
  if (action != Action::NONE) {
    emit_button(action, true);
    emit_button(action, false);
  }
}

void
SDL2Display::draw_y_quick_menu()
{
  float const cx = static_cast<float>(m_size.width()) * 0.5f;
  float const cy = static_cast<float>(m_size.height()) * 0.5f;
  Color const bg(0.0f, 0.0f, 0.0f, 0.65f);
  Color const fg(1.0f, 1.0f, 1.0f, 1.0f);
  m_renderer.draw_fill_rect(geom::frect(geom::fpoint(cx - 90.0f, cy - 70.0f),
                                        geom::fsize(180.0f, 140.0f)), bg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy - 50.0f), "Y+stick menu", fg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy - 28.0f), "Up: Load", fg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy - 12.0f), "Down: Save", fg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy + 4.0f), "Left: Clear", fg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy + 20.0f), "Right: Slow-mo", fg);
  m_renderer.draw_string_centered(geom::fpoint(cx, cy + 44.0f), "Start=Run  Select=UI", fg);
}

void
SDL2Display::handle_controller_axis()
{
  if (!m_controller) {
    return;
  }

  Uint32 const now = SDL_GetTicks();
  float dt = static_cast<float>(now - m_axis_last_ticks) / 1000.0f;
  m_axis_last_ticks = now;
  if (dt <= 0.0f) {
    dt = 1.0f / 60.0f;
  }
  if (dt > 0.1f) {
    dt = 0.1f;
  }

  constexpr float dead = 8000.0f;
  constexpr float max_axis = 32767.0f;
  auto axis_norm = [&](Sint16 raw) -> float {
    float v = static_cast<float>(raw);
    float a = std::fabs(v);
    if (a <= dead) {
      return 0.0f;
    }
    float t = (a - dead) / (max_axis - dead);
    if (t > 1.0f) {
      t = 1.0f;
    }
    return (v < 0.0f ? -t : t);
  };

  float const lx = axis_norm(SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTX));
  float const ly = axis_norm(SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_LEFTY));
  float const rx = axis_norm(SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_RIGHTX));
  float const ry = axis_norm(SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_RIGHTY));

  // Y held: left stick drives quick menu instead of the cursor.
  if (m_y_held) {
    handle_y_quick_menu(lx, ly);
  } else if (lx != 0.0f || ly != 0.0f) {
    float const speed = 280.0f * static_cast<float>(m_size.width()) / 640.0f;
    m_mouse_pos = geom::ipoint(
      m_mouse_pos.x() + static_cast<int>(lx * speed * dt + (lx > 0 ? 0.5f : -0.5f)),
      m_mouse_pos.y() + static_cast<int>(ly * speed * dt + (ly > 0 ? 0.5f : -0.5f)));
    clamp_mouse_pos();
  }

  // Right stick → smooth analog pan (sub-pixel, proportional to deflection).
  if ((rx != 0.0f || ry != 0.0f) && WorldViewWidget::instance()) {
    // Match the old 20px step scale: full deflection ≈ 16 steps/s ≈ 320 px/s.
    float const scroll_speed = 320.0f;
    WorldViewWidget::instance()->scroll_by(
      geom::foffset(rx * scroll_speed * dt, ry * scroll_speed * dt));
  }

  // Triggers → undo / redo (edge-triggered).
  constexpr Sint16 zdead = 16000;
  static bool ltrig = false, rtrig = false;
  {
    Sint16 lt = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT);
    Sint16 rt = SDL_GameControllerGetAxis(m_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
    bool l = lt > zdead;
    bool r = rt > zdead;
    if (l != ltrig) { emit_button(Action::UNDO, l); ltrig = l; }
    if (r != rtrig) { emit_button(Action::REDO, r); rtrig = r; }
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
