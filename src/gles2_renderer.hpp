// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_GLES2_RENDERER_HPP
#define HEADER_CONSTRUO_GLES2_RENDERER_HPP

#include <string>
#include <vector>

#include <geom/point.hpp>
#include <geom/rect.hpp>
#include <geom/size.hpp>

#include "color.hpp"
#include "graphic_context.hpp"

namespace construo {

/** Low-level GLES2 drawing helpers used by SDL2Display (and other
    GLES2-based backends). Owns shaders, a dynamic vertex buffer, and a
    simple 8x8 bitmap font atlas. */
class GLES2Renderer
{
public:
  GLES2Renderer();
  ~GLES2Renderer();

  GLES2Renderer(const GLES2Renderer&) = delete;
  GLES2Renderer& operator=(const GLES2Renderer&) = delete;

  /** Compile shaders, create VBO/VAO-like state, load font atlas.
      Must be called with a current GLES2 context. */
  void init();
  void shutdown();

  void set_viewport(geom::isize const& size);
  geom::isize viewport() const { return m_viewport; }

  void clear(Color const& color = Color(0.0f, 0.0f, 0.0f, 1.0f));

  void draw_line(geom::fpoint const& p1, geom::fpoint const& p2,
                 Color color, float width = 1.0f);
  void draw_lines(std::vector<GraphicContext::Line> const& lines,
                  Color color, float width = 1.0f);
  void draw_rect(geom::frect const& rect, Color color);
  void draw_fill_rect(geom::frect const& rect, Color color);
  void draw_circle(geom::fpoint const& pos, float radius, Color color, int segments = 24);
  void draw_fill_circle(geom::fpoint const& pos, float radius, Color color, int segments = 24);
  void draw_circles(std::vector<GraphicContext::Circle> const& circles, Color color);

  void draw_string(geom::fpoint const& pos, std::string const& str, Color color);
  void draw_string_centered(geom::fpoint const& pos, std::string const& str, Color color);

  void set_clip_rect(geom::frect const& rect);
  void clear_clip_rect();

  void push_quick_draw();
  void pop_quick_draw();

private:
  struct Vertex {
    float x, y;
    float r, g, b, a;
    float u, v;
  };

  void ensure_init() const;
  unsigned compile_shader(unsigned type, char const* source);
  unsigned link_program(unsigned vs, unsigned fs);
  void set_color_uniform(Color const& color);
  void draw_arrays(unsigned mode, std::vector<Vertex> const& verts, bool textured = false);
  void build_font_atlas();

  bool m_ready = false;
  geom::isize m_viewport{800, 600};

  unsigned m_program = 0;
  unsigned m_vao = 0;
  unsigned m_vbo = 0;
  int m_u_screen = -1;
  int m_u_use_tex = -1;
  int m_u_tex = -1;
  int m_a_pos = -1;
  int m_a_color = -1;
  int m_a_uv = -1;

  unsigned m_font_tex = 0;
  int m_glyph_w = 8;
  int m_glyph_h = 8;

  int m_quick_draw_depth = 0;
  bool m_clip_enabled = false;
};

} // namespace construo

#endif

/* EOF */
