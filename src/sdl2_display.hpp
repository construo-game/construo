// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_SDL2_DISPLAY_HPP
#define HEADER_CONSTRUO_SDL2_DISPLAY_HPP

#include <string>
#include <unordered_map>

#include <SDL.h>
#include <geom/point.hpp>
#include <geom/rect.hpp>
#include <geom/size.hpp>

#include "gles2_renderer.hpp"
#include "input_context.hpp"
#include "root_graphic_context.hpp"

namespace construo {

class SDL2Display : public RootGraphicContext,
                    public InputContext
{
public:
  SDL2Display(std::string const& title, int width, int height, bool fullscreen);
  ~SDL2Display() override;

  void draw_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_circles(std::vector<Circle>& circles, Color color) override;
  void draw_fill_circle(geom::fpoint const& pos, float r, Color color) override;
  void draw_fill_rect(geom::frect const& rect, Color color) override;
  void draw_line(geom::fpoint const& p1, geom::fpoint const& p2, Color color, int wide = 0) override;
  void draw_lines(std::vector<Line>& lines, Color color, int wide = 0) override;
  void draw_rect(geom::frect const& rect, Color color) override;
  void draw_string(geom::fpoint const& pos, const std::string& str, Color color) override;
  void draw_string_centered(geom::fpoint const& pos, const std::string& str, Color color) override;

  void clear() override;
  void flip() override;

  geom::frect geometry() const override;

  geom::fpoint get_mouse_pos() const override;

  void run();
  void set_cursor_real(CursorType) override;

  void set_clip_rect(geom::frect const& rect) override;
  void clear_clip_rect() override;

  void push_quick_draw() override;
  void pop_quick_draw() override;

  void enter_fullscreen() override;
  void leave_fullscreen() override;
  void toggle_fullscreen() override;

  void bind_key(SDL_Keycode key, Action action);

  /** Public entry for the Emscripten main-loop trampoline. */
  void process_event_public(SDL_Event const& ev);

private:
  void process_event(SDL_Event const& ev);
  void handle_key(SDL_Keycode key, bool pressed);
  /** Map window-space coordinates to drawable (framebuffer) pixels. */
  geom::ipoint window_to_drawable(int x, int y) const;

  SDL_Window* m_window = nullptr;
  SDL_GLContext m_gl = nullptr;
  GLES2Renderer m_renderer;

  geom::isize m_size;
  geom::ipoint m_mouse_pos;
  bool m_is_fullscreen = false;
  std::string m_title;

  std::unordered_map<SDL_Keycode, Action> m_key_bindings;

public:
  SDL2Display(const SDL2Display&) = delete;
  SDL2Display& operator=(const SDL2Display&) = delete;
};

} // namespace construo

#endif

/* EOF */
