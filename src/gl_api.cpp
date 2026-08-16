// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#define CONSTRUO_GL_API_IMPLEMENTATION
#include "gl_api.hpp"

#include <stdexcept>
#include <string>

#if defined(_WIN32) || defined(WIN32)

namespace construo {
namespace gl_api {

PFN_glActiveTexture p_glActiveTexture = nullptr;
PFN_glAttachShader p_glAttachShader = nullptr;
PFN_glBindBuffer p_glBindBuffer = nullptr;
PFN_glBindTexture p_glBindTexture = nullptr;
PFN_glBlendFunc p_glBlendFunc = nullptr;
PFN_glBufferData p_glBufferData = nullptr;
PFN_glClear p_glClear = nullptr;
PFN_glClearColor p_glClearColor = nullptr;
PFN_glCompileShader p_glCompileShader = nullptr;
PFN_glCreateProgram p_glCreateProgram = nullptr;
PFN_glCreateShader p_glCreateShader = nullptr;
PFN_glDeleteBuffers p_glDeleteBuffers = nullptr;
PFN_glDeleteProgram p_glDeleteProgram = nullptr;
PFN_glDeleteShader p_glDeleteShader = nullptr;
PFN_glDeleteTextures p_glDeleteTextures = nullptr;
PFN_glDisable p_glDisable = nullptr;
PFN_glDisableVertexAttribArray p_glDisableVertexAttribArray = nullptr;
PFN_glDrawArrays p_glDrawArrays = nullptr;
PFN_glEnable p_glEnable = nullptr;
PFN_glEnableVertexAttribArray p_glEnableVertexAttribArray = nullptr;
PFN_glGenBuffers p_glGenBuffers = nullptr;
PFN_glGenTextures p_glGenTextures = nullptr;
PFN_glGetAttribLocation p_glGetAttribLocation = nullptr;
PFN_glGetProgramInfoLog p_glGetProgramInfoLog = nullptr;
PFN_glGetProgramiv p_glGetProgramiv = nullptr;
PFN_glGetShaderInfoLog p_glGetShaderInfoLog = nullptr;
PFN_glGetShaderiv p_glGetShaderiv = nullptr;
PFN_glGetString p_glGetString = nullptr;
PFN_glGetUniformLocation p_glGetUniformLocation = nullptr;
PFN_glLineWidth p_glLineWidth = nullptr;
PFN_glLinkProgram p_glLinkProgram = nullptr;
PFN_glScissor p_glScissor = nullptr;
PFN_glShaderSource p_glShaderSource = nullptr;
PFN_glTexImage2D p_glTexImage2D = nullptr;
PFN_glTexParameteri p_glTexParameteri = nullptr;
PFN_glUniform1f p_glUniform1f = nullptr;
PFN_glUniform1i p_glUniform1i = nullptr;
PFN_glUniform2f p_glUniform2f = nullptr;
PFN_glUseProgram p_glUseProgram = nullptr;
PFN_glVertexAttribPointer p_glVertexAttribPointer = nullptr;
PFN_glViewport p_glViewport = nullptr;
PFN_glGenVertexArrays p_glGenVertexArrays = nullptr;
PFN_glBindVertexArray p_glBindVertexArray = nullptr;
PFN_glDeleteVertexArrays p_glDeleteVertexArrays = nullptr;

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
  p_glActiveTexture = load_sym<PFN_glActiveTexture>("glActiveTexture");
  p_glAttachShader = load_sym<PFN_glAttachShader>("glAttachShader");
  p_glBindBuffer = load_sym<PFN_glBindBuffer>("glBindBuffer");
  p_glBindTexture = load_sym<PFN_glBindTexture>("glBindTexture");
  p_glBlendFunc = load_sym<PFN_glBlendFunc>("glBlendFunc");
  p_glBufferData = load_sym<PFN_glBufferData>("glBufferData");
  p_glClear = load_sym<PFN_glClear>("glClear");
  p_glClearColor = load_sym<PFN_glClearColor>("glClearColor");
  p_glCompileShader = load_sym<PFN_glCompileShader>("glCompileShader");
  p_glCreateProgram = load_sym<PFN_glCreateProgram>("glCreateProgram");
  p_glCreateShader = load_sym<PFN_glCreateShader>("glCreateShader");
  p_glDeleteBuffers = load_sym<PFN_glDeleteBuffers>("glDeleteBuffers");
  p_glDeleteProgram = load_sym<PFN_glDeleteProgram>("glDeleteProgram");
  p_glDeleteShader = load_sym<PFN_glDeleteShader>("glDeleteShader");
  p_glDeleteTextures = load_sym<PFN_glDeleteTextures>("glDeleteTextures");
  p_glDisable = load_sym<PFN_glDisable>("glDisable");
  p_glDisableVertexAttribArray = load_sym<PFN_glDisableVertexAttribArray>("glDisableVertexAttribArray");
  p_glDrawArrays = load_sym<PFN_glDrawArrays>("glDrawArrays");
  p_glEnable = load_sym<PFN_glEnable>("glEnable");
  p_glEnableVertexAttribArray = load_sym<PFN_glEnableVertexAttribArray>("glEnableVertexAttribArray");
  p_glGenBuffers = load_sym<PFN_glGenBuffers>("glGenBuffers");
  p_glGenTextures = load_sym<PFN_glGenTextures>("glGenTextures");
  p_glGetAttribLocation = load_sym<PFN_glGetAttribLocation>("glGetAttribLocation");
  p_glGetProgramInfoLog = load_sym<PFN_glGetProgramInfoLog>("glGetProgramInfoLog");
  p_glGetProgramiv = load_sym<PFN_glGetProgramiv>("glGetProgramiv");
  p_glGetShaderInfoLog = load_sym<PFN_glGetShaderInfoLog>("glGetShaderInfoLog");
  p_glGetShaderiv = load_sym<PFN_glGetShaderiv>("glGetShaderiv");
  p_glGetString = load_sym<PFN_glGetString>("glGetString");
  p_glGetUniformLocation = load_sym<PFN_glGetUniformLocation>("glGetUniformLocation");
  p_glLineWidth = load_sym<PFN_glLineWidth>("glLineWidth");
  p_glLinkProgram = load_sym<PFN_glLinkProgram>("glLinkProgram");
  p_glScissor = load_sym<PFN_glScissor>("glScissor");
  p_glShaderSource = load_sym<PFN_glShaderSource>("glShaderSource");
  p_glTexImage2D = load_sym<PFN_glTexImage2D>("glTexImage2D");
  p_glTexParameteri = load_sym<PFN_glTexParameteri>("glTexParameteri");
  p_glUniform1f = load_sym<PFN_glUniform1f>("glUniform1f");
  p_glUniform1i = load_sym<PFN_glUniform1i>("glUniform1i");
  p_glUniform2f = load_sym<PFN_glUniform2f>("glUniform2f");
  p_glUseProgram = load_sym<PFN_glUseProgram>("glUseProgram");
  p_glVertexAttribPointer = load_sym<PFN_glVertexAttribPointer>("glVertexAttribPointer");
  p_glViewport = load_sym<PFN_glViewport>("glViewport");
  // Core-profile VAO entry points (required for GL 3.3 core draws).
  p_glGenVertexArrays = load_sym<PFN_glGenVertexArrays>("glGenVertexArrays");
  p_glBindVertexArray = load_sym<PFN_glBindVertexArray>("glBindVertexArray");
  p_glDeleteVertexArrays = load_sym<PFN_glDeleteVertexArrays>("glDeleteVertexArrays");
}

} // namespace gl_api
} // namespace construo

#endif // _WIN32
