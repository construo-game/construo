// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "gl_api.hpp"

#include <stdexcept>
#include <string>

#if defined(_WIN32) || defined(WIN32)

namespace construo {
namespace gl_api {

PFN_glActiveTexture glActiveTexture = nullptr;
PFN_glAttachShader glAttachShader = nullptr;
PFN_glBindBuffer glBindBuffer = nullptr;
PFN_glBindTexture glBindTexture = nullptr;
PFN_glBlendFunc glBlendFunc = nullptr;
PFN_glBufferData glBufferData = nullptr;
PFN_glClear glClear = nullptr;
PFN_glClearColor glClearColor = nullptr;
PFN_glCompileShader glCompileShader = nullptr;
PFN_glCreateProgram glCreateProgram = nullptr;
PFN_glCreateShader glCreateShader = nullptr;
PFN_glDeleteBuffers glDeleteBuffers = nullptr;
PFN_glDeleteProgram glDeleteProgram = nullptr;
PFN_glDeleteShader glDeleteShader = nullptr;
PFN_glDeleteTextures glDeleteTextures = nullptr;
PFN_glDisable glDisable = nullptr;
PFN_glDisableVertexAttribArray glDisableVertexAttribArray = nullptr;
PFN_glDrawArrays glDrawArrays = nullptr;
PFN_glEnable glEnable = nullptr;
PFN_glEnableVertexAttribArray glEnableVertexAttribArray = nullptr;
PFN_glGenBuffers glGenBuffers = nullptr;
PFN_glGenTextures glGenTextures = nullptr;
PFN_glGetAttribLocation glGetAttribLocation = nullptr;
PFN_glGetProgramInfoLog glGetProgramInfoLog = nullptr;
PFN_glGetProgramiv glGetProgramiv = nullptr;
PFN_glGetShaderInfoLog glGetShaderInfoLog = nullptr;
PFN_glGetShaderiv glGetShaderiv = nullptr;
PFN_glGetString glGetString = nullptr;
PFN_glGetUniformLocation glGetUniformLocation = nullptr;
PFN_glLineWidth glLineWidth = nullptr;
PFN_glLinkProgram glLinkProgram = nullptr;
PFN_glScissor glScissor = nullptr;
PFN_glShaderSource glShaderSource = nullptr;
PFN_glTexImage2D glTexImage2D = nullptr;
PFN_glTexParameteri glTexParameteri = nullptr;
PFN_glUniform1f glUniform1f = nullptr;
PFN_glUniform1i glUniform1i = nullptr;
PFN_glUniform2f glUniform2f = nullptr;
PFN_glUseProgram glUseProgram = nullptr;
PFN_glVertexAttribPointer glVertexAttribPointer = nullptr;
PFN_glViewport glViewport = nullptr;

namespace {

template<typename T>
T load_sym(char const* name)
{
  T fn = reinterpret_cast<T>(SDL_GL_GetProcAddress(name));
  if (!fn) {
    throw std::runtime_error(std::string("SDL_GL_GetProcAddress failed for ") + name);
  }
  return fn;
}

} // namespace

void load()
{
  glActiveTexture = load_sym<PFN_glActiveTexture>("glActiveTexture");
  glAttachShader = load_sym<PFN_glAttachShader>("glAttachShader");
  glBindBuffer = load_sym<PFN_glBindBuffer>("glBindBuffer");
  glBindTexture = load_sym<PFN_glBindTexture>("glBindTexture");
  glBlendFunc = load_sym<PFN_glBlendFunc>("glBlendFunc");
  glBufferData = load_sym<PFN_glBufferData>("glBufferData");
  glClear = load_sym<PFN_glClear>("glClear");
  glClearColor = load_sym<PFN_glClearColor>("glClearColor");
  glCompileShader = load_sym<PFN_glCompileShader>("glCompileShader");
  glCreateProgram = load_sym<PFN_glCreateProgram>("glCreateProgram");
  glCreateShader = load_sym<PFN_glCreateShader>("glCreateShader");
  glDeleteBuffers = load_sym<PFN_glDeleteBuffers>("glDeleteBuffers");
  glDeleteProgram = load_sym<PFN_glDeleteProgram>("glDeleteProgram");
  glDeleteShader = load_sym<PFN_glDeleteShader>("glDeleteShader");
  glDeleteTextures = load_sym<PFN_glDeleteTextures>("glDeleteTextures");
  glDisable = load_sym<PFN_glDisable>("glDisable");
  glDisableVertexAttribArray = load_sym<PFN_glDisableVertexAttribArray>("glDisableVertexAttribArray");
  glDrawArrays = load_sym<PFN_glDrawArrays>("glDrawArrays");
  glEnable = load_sym<PFN_glEnable>("glEnable");
  glEnableVertexAttribArray = load_sym<PFN_glEnableVertexAttribArray>("glEnableVertexAttribArray");
  glGenBuffers = load_sym<PFN_glGenBuffers>("glGenBuffers");
  glGenTextures = load_sym<PFN_glGenTextures>("glGenTextures");
  glGetAttribLocation = load_sym<PFN_glGetAttribLocation>("glGetAttribLocation");
  glGetProgramInfoLog = load_sym<PFN_glGetProgramInfoLog>("glGetProgramInfoLog");
  glGetProgramiv = load_sym<PFN_glGetProgramiv>("glGetProgramiv");
  glGetShaderInfoLog = load_sym<PFN_glGetShaderInfoLog>("glGetShaderInfoLog");
  glGetShaderiv = load_sym<PFN_glGetShaderiv>("glGetShaderiv");
  glGetString = load_sym<PFN_glGetString>("glGetString");
  glGetUniformLocation = load_sym<PFN_glGetUniformLocation>("glGetUniformLocation");
  glLineWidth = load_sym<PFN_glLineWidth>("glLineWidth");
  glLinkProgram = load_sym<PFN_glLinkProgram>("glLinkProgram");
  glScissor = load_sym<PFN_glScissor>("glScissor");
  glShaderSource = load_sym<PFN_glShaderSource>("glShaderSource");
  glTexImage2D = load_sym<PFN_glTexImage2D>("glTexImage2D");
  glTexParameteri = load_sym<PFN_glTexParameteri>("glTexParameteri");
  glUniform1f = load_sym<PFN_glUniform1f>("glUniform1f");
  glUniform1i = load_sym<PFN_glUniform1i>("glUniform1i");
  glUniform2f = load_sym<PFN_glUniform2f>("glUniform2f");
  glUseProgram = load_sym<PFN_glUseProgram>("glUseProgram");
  glVertexAttribPointer = load_sym<PFN_glVertexAttribPointer>("glVertexAttribPointer");
  glViewport = load_sym<PFN_glViewport>("glViewport");
}

} // namespace gl_api
} // namespace construo

#endif // _WIN32
