// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_GL_API_HPP
#define HEADER_CONSTRUO_GL_API_HPP

#include <SDL.h>

#if defined(_WIN32) || defined(WIN32)

// Types only from Khronos headers (no dllimport).
#  ifndef KHRONOS_STATIC
#    define KHRONOS_STATIC
#  endif
#  include <GLES2/gl2.h>

// Replace link-time GL symbols with SDL-loaded function pointers.
#  undef glActiveTexture
#  undef glAttachShader
#  undef glBindBuffer
#  undef glBindTexture
#  undef glBlendFunc
#  undef glBufferData
#  undef glClear
#  undef glClearColor
#  undef glCompileShader
#  undef glCreateProgram
#  undef glCreateShader
#  undef glDeleteBuffers
#  undef glDeleteProgram
#  undef glDeleteShader
#  undef glDeleteTextures
#  undef glDisable
#  undef glDisableVertexAttribArray
#  undef glDrawArrays
#  undef glEnable
#  undef glEnableVertexAttribArray
#  undef glGenBuffers
#  undef glGenTextures
#  undef glGetAttribLocation
#  undef glGetProgramInfoLog
#  undef glGetProgramiv
#  undef glGetShaderInfoLog
#  undef glGetShaderiv
#  undef glGetString
#  undef glGetUniformLocation
#  undef glLineWidth
#  undef glLinkProgram
#  undef glScissor
#  undef glShaderSource
#  undef glTexImage2D
#  undef glTexParameteri
#  undef glUniform1f
#  undef glUniform1i
#  undef glUniform2f
#  undef glUseProgram
#  undef glVertexAttribPointer
#  undef glViewport

namespace construo {
namespace gl_api {

void load();

using PFN_glActiveTexture = void (GL_APIENTRY*)(GLenum texture);
extern PFN_glActiveTexture glActiveTexture;
using PFN_glAttachShader = void (GL_APIENTRY*)(GLuint program, GLuint shader);
extern PFN_glAttachShader glAttachShader;
using PFN_glBindBuffer = void (GL_APIENTRY*)(GLenum target, GLuint buffer);
extern PFN_glBindBuffer glBindBuffer;
using PFN_glBindTexture = void (GL_APIENTRY*)(GLenum target, GLuint texture);
extern PFN_glBindTexture glBindTexture;
using PFN_glBlendFunc = void (GL_APIENTRY*)(GLenum sfactor, GLenum dfactor);
extern PFN_glBlendFunc glBlendFunc;
using PFN_glBufferData = void (GL_APIENTRY*)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
extern PFN_glBufferData glBufferData;
using PFN_glClear = void (GL_APIENTRY*)(GLbitfield mask);
extern PFN_glClear glClear;
using PFN_glClearColor = void (GL_APIENTRY*)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern PFN_glClearColor glClearColor;
using PFN_glCompileShader = void (GL_APIENTRY*)(GLuint shader);
extern PFN_glCompileShader glCompileShader;
using PFN_glCreateProgram = GLuint (GL_APIENTRY*)(void);
extern PFN_glCreateProgram glCreateProgram;
using PFN_glCreateShader = GLuint (GL_APIENTRY*)(GLenum type);
extern PFN_glCreateShader glCreateShader;
using PFN_glDeleteBuffers = void (GL_APIENTRY*)(GLsizei n, const GLuint* buffers);
extern PFN_glDeleteBuffers glDeleteBuffers;
using PFN_glDeleteProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glDeleteProgram glDeleteProgram;
using PFN_glDeleteShader = void (GL_APIENTRY*)(GLuint shader);
extern PFN_glDeleteShader glDeleteShader;
using PFN_glDeleteTextures = void (GL_APIENTRY*)(GLsizei n, const GLuint* textures);
extern PFN_glDeleteTextures glDeleteTextures;
using PFN_glDisable = void (GL_APIENTRY*)(GLenum cap);
extern PFN_glDisable glDisable;
using PFN_glDisableVertexAttribArray = void (GL_APIENTRY*)(GLuint index);
extern PFN_glDisableVertexAttribArray glDisableVertexAttribArray;
using PFN_glDrawArrays = void (GL_APIENTRY*)(GLenum mode, GLint first, GLsizei count);
extern PFN_glDrawArrays glDrawArrays;
using PFN_glEnable = void (GL_APIENTRY*)(GLenum cap);
extern PFN_glEnable glEnable;
using PFN_glEnableVertexAttribArray = void (GL_APIENTRY*)(GLuint index);
extern PFN_glEnableVertexAttribArray glEnableVertexAttribArray;
using PFN_glGenBuffers = void (GL_APIENTRY*)(GLsizei n, GLuint* buffers);
extern PFN_glGenBuffers glGenBuffers;
using PFN_glGenTextures = void (GL_APIENTRY*)(GLsizei n, GLuint* textures);
extern PFN_glGenTextures glGenTextures;
using PFN_glGetAttribLocation = GLint (GL_APIENTRY*)(GLuint program, const GLchar* name);
extern PFN_glGetAttribLocation glGetAttribLocation;
using PFN_glGetProgramInfoLog = void (GL_APIENTRY*)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFN_glGetProgramInfoLog glGetProgramInfoLog;
using PFN_glGetProgramiv = void (GL_APIENTRY*)(GLuint program, GLenum pname, GLint* params);
extern PFN_glGetProgramiv glGetProgramiv;
using PFN_glGetShaderInfoLog = void (GL_APIENTRY*)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFN_glGetShaderInfoLog glGetShaderInfoLog;
using PFN_glGetShaderiv = void (GL_APIENTRY*)(GLuint shader, GLenum pname, GLint* params);
extern PFN_glGetShaderiv glGetShaderiv;
using PFN_glGetString = const GLubyte* (GL_APIENTRY*)(GLenum name);
extern PFN_glGetString glGetString;
using PFN_glGetUniformLocation = GLint (GL_APIENTRY*)(GLuint program, const GLchar* name);
extern PFN_glGetUniformLocation glGetUniformLocation;
using PFN_glLineWidth = void (GL_APIENTRY*)(GLfloat width);
extern PFN_glLineWidth glLineWidth;
using PFN_glLinkProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glLinkProgram glLinkProgram;
using PFN_glScissor = void (GL_APIENTRY*)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFN_glScissor glScissor;
using PFN_glShaderSource = void (GL_APIENTRY*)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
extern PFN_glShaderSource glShaderSource;
using PFN_glTexImage2D = void (GL_APIENTRY*)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
extern PFN_glTexImage2D glTexImage2D;
using PFN_glTexParameteri = void (GL_APIENTRY*)(GLenum target, GLenum pname, GLint param);
extern PFN_glTexParameteri glTexParameteri;
using PFN_glUniform1f = void (GL_APIENTRY*)(GLint location, GLfloat v0);
extern PFN_glUniform1f glUniform1f;
using PFN_glUniform1i = void (GL_APIENTRY*)(GLint location, GLint v0);
extern PFN_glUniform1i glUniform1i;
using PFN_glUniform2f = void (GL_APIENTRY*)(GLint location, GLfloat v0, GLfloat v1);
extern PFN_glUniform2f glUniform2f;
using PFN_glUseProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glUseProgram glUseProgram;
using PFN_glVertexAttribPointer = void (GL_APIENTRY*)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
extern PFN_glVertexAttribPointer glVertexAttribPointer;
using PFN_glViewport = void (GL_APIENTRY*)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFN_glViewport glViewport;

} // namespace gl_api
} // namespace construo

// Bring loaded entry points into the global names used by gles2_renderer.
#define glActiveTexture ::construo::gl_api::glActiveTexture
#define glAttachShader ::construo::gl_api::glAttachShader
#define glBindBuffer ::construo::gl_api::glBindBuffer
#define glBindTexture ::construo::gl_api::glBindTexture
#define glBlendFunc ::construo::gl_api::glBlendFunc
#define glBufferData ::construo::gl_api::glBufferData
#define glClear ::construo::gl_api::glClear
#define glClearColor ::construo::gl_api::glClearColor
#define glCompileShader ::construo::gl_api::glCompileShader
#define glCreateProgram ::construo::gl_api::glCreateProgram
#define glCreateShader ::construo::gl_api::glCreateShader
#define glDeleteBuffers ::construo::gl_api::glDeleteBuffers
#define glDeleteProgram ::construo::gl_api::glDeleteProgram
#define glDeleteShader ::construo::gl_api::glDeleteShader
#define glDeleteTextures ::construo::gl_api::glDeleteTextures
#define glDisable ::construo::gl_api::glDisable
#define glDisableVertexAttribArray ::construo::gl_api::glDisableVertexAttribArray
#define glDrawArrays ::construo::gl_api::glDrawArrays
#define glEnable ::construo::gl_api::glEnable
#define glEnableVertexAttribArray ::construo::gl_api::glEnableVertexAttribArray
#define glGenBuffers ::construo::gl_api::glGenBuffers
#define glGenTextures ::construo::gl_api::glGenTextures
#define glGetAttribLocation ::construo::gl_api::glGetAttribLocation
#define glGetProgramInfoLog ::construo::gl_api::glGetProgramInfoLog
#define glGetProgramiv ::construo::gl_api::glGetProgramiv
#define glGetShaderInfoLog ::construo::gl_api::glGetShaderInfoLog
#define glGetShaderiv ::construo::gl_api::glGetShaderiv
#define glGetString ::construo::gl_api::glGetString
#define glGetUniformLocation ::construo::gl_api::glGetUniformLocation
#define glLineWidth ::construo::gl_api::glLineWidth
#define glLinkProgram ::construo::gl_api::glLinkProgram
#define glScissor ::construo::gl_api::glScissor
#define glShaderSource ::construo::gl_api::glShaderSource
#define glTexImage2D ::construo::gl_api::glTexImage2D
#define glTexParameteri ::construo::gl_api::glTexParameteri
#define glUniform1f ::construo::gl_api::glUniform1f
#define glUniform1i ::construo::gl_api::glUniform1i
#define glUniform2f ::construo::gl_api::glUniform2f
#define glUseProgram ::construo::gl_api::glUseProgram
#define glVertexAttribPointer ::construo::gl_api::glVertexAttribPointer
#define glViewport ::construo::gl_api::glViewport

#else // !_WIN32

#  include <SDL_opengles2.h>

namespace construo {
namespace gl_api {
inline void load() {}
} // namespace gl_api
} // namespace construo

#endif // _WIN32

#endif

