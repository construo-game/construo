#include <cstddef>
// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gles2_renderer.hpp"

#include <cmath>
#include <cstring>
#include <stdexcept>
#include <vector>

#include <logmich/log.hpp>

// GLES2 API via SDL's portable header (works for Emscripten, Android, desktop ES).
#include <SDL_opengles2.h>

namespace construo {

namespace {

char const* k_vs = R"(
attribute vec2 a_pos;
attribute vec4 a_color;
attribute vec2 a_uv;
uniform vec2 u_screen;
varying vec4 v_color;
varying vec2 v_uv;
void main() {
  vec2 ndc = vec2(
    (a_pos.x / u_screen.x) * 2.0 - 1.0,
    1.0 - (a_pos.y / u_screen.y) * 2.0
  );
  gl_Position = vec4(ndc, 0.0, 1.0);
  v_color = a_color;
  v_uv = a_uv;
}
)";

char const* k_fs = R"(
precision mediump float;
varying vec4 v_color;
varying vec2 v_uv;
uniform sampler2D u_tex;
uniform int u_use_tex;
void main() {
  if (u_use_tex != 0) {
    float a = texture2D(u_tex, v_uv).a;
    gl_FragColor = vec4(v_color.rgb, v_color.a * a);
  } else {
    gl_FragColor = v_color;
  }
}
)";

// Minimal 8x8 ASCII font (space..~), 1 bit per pixel, row-major MSB left.
// Generated as a compact glyph strip; missing glyphs render empty.
constexpr int k_first_char = 32;
constexpr int k_num_chars = 95;
constexpr int k_glyph = 8;

// Each glyph: 8 bytes (one per row). Only a usable subset is filled;
// remaining glyphs stay blank.
#include "gles2_font_data.inc"

} // namespace

GLES2Renderer::GLES2Renderer() = default;

GLES2Renderer::~GLES2Renderer()
{
  shutdown();
}

void
GLES2Renderer::ensure_init() const
{
  if (!m_ready) {
    throw std::runtime_error("GLES2Renderer used before init()");
  }
}

unsigned
GLES2Renderer::compile_shader(unsigned type, char const* source)
{
  unsigned s = glCreateShader(type);
  glShaderSource(s, 1, &source, nullptr);
  glCompileShader(s);
  int ok = 0;
  glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetShaderInfoLog(s, sizeof(log), nullptr, log);
    glDeleteShader(s);
    throw std::runtime_error(std::string("GLES2 shader compile failed: ") + log);
  }
  return s;
}

unsigned
GLES2Renderer::link_program(unsigned vs, unsigned fs)
{
  unsigned p = glCreateProgram();
  glAttachShader(p, vs);
  glAttachShader(p, fs);
  glLinkProgram(p);
  int ok = 0;
  glGetProgramiv(p, GL_LINK_STATUS, &ok);
  if (!ok) {
    char log[512];
    glGetProgramInfoLog(p, sizeof(log), nullptr, log);
    glDeleteProgram(p);
    throw std::runtime_error(std::string("GLES2 program link failed: ") + log);
  }
  return p;
}

void
GLES2Renderer::init()
{
  if (m_ready) {
    return;
  }

  unsigned vs = compile_shader(GL_VERTEX_SHADER, k_vs);
  unsigned fs = compile_shader(GL_FRAGMENT_SHADER, k_fs);
  m_program = link_program(vs, fs);
  glDeleteShader(vs);
  glDeleteShader(fs);

  m_a_pos = glGetAttribLocation(m_program, "a_pos");
  m_a_color = glGetAttribLocation(m_program, "a_color");
  m_a_uv = glGetAttribLocation(m_program, "a_uv");
  m_u_screen = glGetUniformLocation(m_program, "u_screen");
  m_u_use_tex = glGetUniformLocation(m_program, "u_use_tex");
  m_u_tex = glGetUniformLocation(m_program, "u_tex");

  glGenBuffers(1, &m_vbo);

  build_font_atlas();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);

  m_ready = true;
  log_info("GLES2Renderer initialized");
}

void
GLES2Renderer::shutdown()
{
  if (!m_ready) {
    return;
  }
  if (m_font_tex) {
    glDeleteTextures(1, &m_font_tex);
    m_font_tex = 0;
  }
  if (m_vbo) {
    glDeleteBuffers(1, &m_vbo);
    m_vbo = 0;
  }
  if (m_program) {
    glDeleteProgram(m_program);
    m_program = 0;
  }
  m_ready = false;
}

void
GLES2Renderer::build_font_atlas()
{
  // Atlas: k_num_chars * 8 wide, 8 tall, single alpha channel.
  int const atlas_w = k_num_chars * k_glyph;
  int const atlas_h = k_glyph;
  std::vector<unsigned char> pixels(static_cast<size_t>(atlas_w * atlas_h), 0);

  for (int c = 0; c < k_num_chars; ++c) {
    unsigned char const* glyph = &gles2_font_glyphs[c * k_glyph];
    for (int row = 0; row < k_glyph; ++row) {
      unsigned char bits = glyph[row];
      for (int col = 0; col < k_glyph; ++col) {
        if (bits & (0x80 >> col)) {
          pixels[static_cast<size_t>(row * atlas_w + c * k_glyph + col)] = 255;
        }
      }
    }
  }

  glGenTextures(1, &m_font_tex);
  glBindTexture(GL_TEXTURE_2D, m_font_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, atlas_w, atlas_h, 0,
               GL_ALPHA, GL_UNSIGNED_BYTE, pixels.data());
  glBindTexture(GL_TEXTURE_2D, 0);
}

void
GLES2Renderer::set_viewport(geom::isize const& size)
{
  m_viewport = size;
  glViewport(0, 0, size.width(), size.height());
}

void
GLES2Renderer::clear(Color const& color)
{
  ensure_init();
  glClearColor(color.r, color.g, color.b, color.a);
  glClear(GL_COLOR_BUFFER_BIT);
}

void
GLES2Renderer::draw_arrays(unsigned mode, std::vector<Vertex> const& verts, bool textured)
{
  if (verts.empty()) {
    return;
  }
  ensure_init();

  glUseProgram(m_program);
  glUniform2f(m_u_screen,
              static_cast<float>(m_viewport.width()),
              static_cast<float>(m_viewport.height()));
  glUniform1i(m_u_use_tex, textured ? 1 : 0);

  if (textured) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_font_tex);
    glUniform1i(m_u_tex, 0);
  }

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               static_cast<GLsizeiptr>(verts.size() * sizeof(Vertex)),
               verts.data(), GL_DYNAMIC_DRAW);

  glEnableVertexAttribArray(static_cast<GLuint>(m_a_pos));
  glVertexAttribPointer(static_cast<GLuint>(m_a_pos), 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, x)));
  glEnableVertexAttribArray(static_cast<GLuint>(m_a_color));
  glVertexAttribPointer(static_cast<GLuint>(m_a_color), 4, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, r)));
  glEnableVertexAttribArray(static_cast<GLuint>(m_a_uv));
  glVertexAttribPointer(static_cast<GLuint>(m_a_uv), 2, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, u)));

  glDrawArrays(mode, 0, static_cast<GLsizei>(verts.size()));

  glDisableVertexAttribArray(static_cast<GLuint>(m_a_pos));
  glDisableVertexAttribArray(static_cast<GLuint>(m_a_color));
  glDisableVertexAttribArray(static_cast<GLuint>(m_a_uv));
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  if (textured) {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
  glUseProgram(0);
}

void
GLES2Renderer::draw_line(geom::fpoint const& p1, geom::fpoint const& p2,
                         Color color, float width)
{
  if (width <= 1.0f && m_quick_draw_depth > 0) {
    std::vector<Vertex> v = {
      {p1.x(), p1.y(), color.r, color.g, color.b, color.a, 0, 0},
      {p2.x(), p2.y(), color.r, color.g, color.b, color.a, 0, 0},
    };
    draw_arrays(GL_LINES, v);
    return;
  }

  // Thick line as a quad.
  float dx = p2.x() - p1.x();
  float dy = p2.y() - p1.y();
  float len = std::sqrt(dx * dx + dy * dy);
  if (len < 1e-4f) {
    return;
  }
  float nx = -dy / len * (width * 0.5f);
  float ny = dx / len * (width * 0.5f);

  std::vector<Vertex> v = {
    {p1.x() + nx, p1.y() + ny, color.r, color.g, color.b, color.a, 0, 0},
    {p1.x() - nx, p1.y() - ny, color.r, color.g, color.b, color.a, 0, 0},
    {p2.x() + nx, p2.y() + ny, color.r, color.g, color.b, color.a, 0, 0},
    {p2.x() - nx, p2.y() - ny, color.r, color.g, color.b, color.a, 0, 0},
  };
  // Two triangles
  std::vector<Vertex> tri = {v[0], v[1], v[2], v[2], v[1], v[3]};
  draw_arrays(GL_TRIANGLES, tri);
}

void
GLES2Renderer::draw_lines(std::vector<GraphicContext::Line> const& lines,
                          Color color, float width)
{
  for (auto const& l : lines) {
    draw_line(l.p1, l.p2, color, width);
  }
}

void
GLES2Renderer::draw_rect(geom::frect const& rect, Color color)
{
  geom::fpoint p1(rect.left(), rect.top());
  geom::fpoint p2(rect.right(), rect.top());
  geom::fpoint p3(rect.right(), rect.bottom());
  geom::fpoint p4(rect.left(), rect.bottom());
  draw_line(p1, p2, color, 1.0f);
  draw_line(p2, p3, color, 1.0f);
  draw_line(p3, p4, color, 1.0f);
  draw_line(p4, p1, color, 1.0f);
}

void
GLES2Renderer::draw_fill_rect(geom::frect const& rect, Color color)
{
  std::vector<Vertex> v = {
    {rect.left(), rect.top(), color.r, color.g, color.b, color.a, 0, 0},
    {rect.right(), rect.top(), color.r, color.g, color.b, color.a, 0, 0},
    {rect.right(), rect.bottom(), color.r, color.g, color.b, color.a, 0, 0},
    {rect.left(), rect.top(), color.r, color.g, color.b, color.a, 0, 0},
    {rect.right(), rect.bottom(), color.r, color.g, color.b, color.a, 0, 0},
    {rect.left(), rect.bottom(), color.r, color.g, color.b, color.a, 0, 0},
  };
  draw_arrays(GL_TRIANGLES, v);
}

void
GLES2Renderer::draw_circle(geom::fpoint const& pos, float radius, Color color, int segments)
{
  if (segments < 3) {
    segments = 3;
  }
  std::vector<Vertex> v;
  v.reserve(static_cast<size_t>(segments * 2));
  for (int i = 0; i < segments; ++i) {
    float a0 = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(segments);
    float a1 = static_cast<float>(i + 1) * 2.0f * 3.14159265f / static_cast<float>(segments);
    v.push_back({pos.x() + std::cos(a0) * radius, pos.y() + std::sin(a0) * radius,
                 color.r, color.g, color.b, color.a, 0, 0});
    v.push_back({pos.x() + std::cos(a1) * radius, pos.y() + std::sin(a1) * radius,
                 color.r, color.g, color.b, color.a, 0, 0});
  }
  draw_arrays(GL_LINES, v);
}

void
GLES2Renderer::draw_fill_circle(geom::fpoint const& pos, float radius, Color color, int segments)
{
  if (segments < 3) {
    segments = 3;
  }
  std::vector<Vertex> v;
  v.reserve(static_cast<size_t>(segments * 3));
  for (int i = 0; i < segments; ++i) {
    float a0 = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(segments);
    float a1 = static_cast<float>(i + 1) * 2.0f * 3.14159265f / static_cast<float>(segments);
    v.push_back({pos.x(), pos.y(), color.r, color.g, color.b, color.a, 0, 0});
    v.push_back({pos.x() + std::cos(a0) * radius, pos.y() + std::sin(a0) * radius,
                 color.r, color.g, color.b, color.a, 0, 0});
    v.push_back({pos.x() + std::cos(a1) * radius, pos.y() + std::sin(a1) * radius,
                 color.r, color.g, color.b, color.a, 0, 0});
  }
  draw_arrays(GL_TRIANGLES, v);
}

void
GLES2Renderer::draw_circles(std::vector<GraphicContext::Circle> const& circles, Color color)
{
  for (auto const& c : circles) {
    draw_circle(c.pos, c.r, color);
  }
}

void
GLES2Renderer::draw_string(geom::fpoint const& pos, std::string const& str, Color color)
{
  float x = pos.x();
  float y = pos.y();
  float const atlas_w = static_cast<float>(k_num_chars * k_glyph);
  float const gw = static_cast<float>(m_glyph_w);
  float const gh = static_cast<float>(m_glyph_h);

  std::vector<Vertex> v;
  v.reserve(str.size() * 6);

  for (unsigned char ch : str) {
    if (ch == '\n') {
      x = pos.x();
      y += gh;
      continue;
    }
    int idx = static_cast<int>(ch) - k_first_char;
    if (idx < 0 || idx >= k_num_chars) {
      x += gw;
      continue;
    }
    float u0 = (static_cast<float>(idx) * gw) / atlas_w;
    float u1 = (static_cast<float>(idx + 1) * gw) / atlas_w;
    float v0 = 0.0f;
    float v1 = 1.0f;

    float x0 = x;
    float y0 = y;
    float x1 = x + gw;
    float y1 = y + gh;

    v.push_back({x0, y0, color.r, color.g, color.b, color.a, u0, v0});
    v.push_back({x1, y0, color.r, color.g, color.b, color.a, u1, v0});
    v.push_back({x1, y1, color.r, color.g, color.b, color.a, u1, v1});
    v.push_back({x0, y0, color.r, color.g, color.b, color.a, u0, v0});
    v.push_back({x1, y1, color.r, color.g, color.b, color.a, u1, v1});
    v.push_back({x0, y1, color.r, color.g, color.b, color.a, u0, v1});

    x += gw;
  }
  draw_arrays(GL_TRIANGLES, v, true);
}

void
GLES2Renderer::draw_string_centered(geom::fpoint const& pos, std::string const& str, Color color)
{
  float width = static_cast<float>(str.size()) * static_cast<float>(m_glyph_w);
  draw_string(geom::fpoint(pos.x() - width * 0.5f, pos.y()), str, color);
}

void
GLES2Renderer::set_clip_rect(geom::frect const& rect)
{
  ensure_init();
  glEnable(GL_SCISSOR_TEST);
  // GLES scissor origin is bottom-left; our coords are top-left.
  int x = static_cast<int>(rect.left());
  int h = static_cast<int>(rect.height());
  int y = m_viewport.height() - static_cast<int>(rect.top()) - h;
  int w = static_cast<int>(rect.width());
  glScissor(x, y, w, h);
  m_clip_enabled = true;
}

void
GLES2Renderer::clear_clip_rect()
{
  if (m_clip_enabled) {
    glDisable(GL_SCISSOR_TEST);
    m_clip_enabled = false;
  }
}

void
GLES2Renderer::push_quick_draw()
{
  ++m_quick_draw_depth;
}

void
GLES2Renderer::pop_quick_draw()
{
  if (m_quick_draw_depth > 0) {
    --m_quick_draw_depth;
  }
}

} // namespace construo

/* EOF */
