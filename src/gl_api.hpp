// SPDX-FileCopyrightText: 2026 Ingo Ruhnke <grumbel@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HEADER_CONSTRUO_GL_API_HPP
#define HEADER_CONSTRUO_GL_API_HPP

#include <SDL.h>

#if defined(_WIN32) || defined(WIN32)

#  ifndef KHRONOS_STATIC
#    define KHRONOS_STATIC
#  endif
#  include <GLES2/gl2.h>

namespace construo {
namespace gl_api {

void load();

using PFN_glActiveTexture = void (GL_APIENTRY*)(GLenum texture);
extern PFN_glActiveTexture p_glActiveTexture;
using PFN_glAttachShader = void (GL_APIENTRY*)(GLuint program, GLuint shader);
extern PFN_glAttachShader p_glAttachShader;
using PFN_glBindBuffer = void (GL_APIENTRY*)(GLenum target, GLuint buffer);
extern PFN_glBindBuffer p_glBindBuffer;
using PFN_glBindTexture = void (GL_APIENTRY*)(GLenum target, GLuint texture);
extern PFN_glBindTexture p_glBindTexture;
using PFN_glBlendFunc = void (GL_APIENTRY*)(GLenum sfactor, GLenum dfactor);
extern PFN_glBlendFunc p_glBlendFunc;
using PFN_glBufferData = void (GL_APIENTRY*)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
extern PFN_glBufferData p_glBufferData;
using PFN_glClear = void (GL_APIENTRY*)(GLbitfield mask);
extern PFN_glClear p_glClear;
using PFN_glClearColor = void (GL_APIENTRY*)(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
extern PFN_glClearColor p_glClearColor;
using PFN_glCompileShader = void (GL_APIENTRY*)(GLuint shader);
extern PFN_glCompileShader p_glCompileShader;
using PFN_glCreateProgram = GLuint (GL_APIENTRY*)(void);
extern PFN_glCreateProgram p_glCreateProgram;
using PFN_glCreateShader = GLuint (GL_APIENTRY*)(GLenum type);
extern PFN_glCreateShader p_glCreateShader;
using PFN_glDeleteBuffers = void (GL_APIENTRY*)(GLsizei n, const GLuint* buffers);
extern PFN_glDeleteBuffers p_glDeleteBuffers;
using PFN_glDeleteProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glDeleteProgram p_glDeleteProgram;
using PFN_glDeleteShader = void (GL_APIENTRY*)(GLuint shader);
extern PFN_glDeleteShader p_glDeleteShader;
using PFN_glDeleteTextures = void (GL_APIENTRY*)(GLsizei n, const GLuint* textures);
extern PFN_glDeleteTextures p_glDeleteTextures;
using PFN_glDisable = void (GL_APIENTRY*)(GLenum cap);
extern PFN_glDisable p_glDisable;
using PFN_glDisableVertexAttribArray = void (GL_APIENTRY*)(GLuint index);
extern PFN_glDisableVertexAttribArray p_glDisableVertexAttribArray;
using PFN_glDrawArrays = void (GL_APIENTRY*)(GLenum mode, GLint first, GLsizei count);
extern PFN_glDrawArrays p_glDrawArrays;
using PFN_glEnable = void (GL_APIENTRY*)(GLenum cap);
extern PFN_glEnable p_glEnable;
using PFN_glEnableVertexAttribArray = void (GL_APIENTRY*)(GLuint index);
extern PFN_glEnableVertexAttribArray p_glEnableVertexAttribArray;
using PFN_glGenBuffers = void (GL_APIENTRY*)(GLsizei n, GLuint* buffers);
extern PFN_glGenBuffers p_glGenBuffers;
using PFN_glGenTextures = void (GL_APIENTRY*)(GLsizei n, GLuint* textures);
extern PFN_glGenTextures p_glGenTextures;
using PFN_glGetAttribLocation = GLint (GL_APIENTRY*)(GLuint program, const GLchar* name);
extern PFN_glGetAttribLocation p_glGetAttribLocation;
using PFN_glGetProgramInfoLog = void (GL_APIENTRY*)(GLuint program, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFN_glGetProgramInfoLog p_glGetProgramInfoLog;
using PFN_glGetProgramiv = void (GL_APIENTRY*)(GLuint program, GLenum pname, GLint* params);
extern PFN_glGetProgramiv p_glGetProgramiv;
using PFN_glGetShaderInfoLog = void (GL_APIENTRY*)(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
extern PFN_glGetShaderInfoLog p_glGetShaderInfoLog;
using PFN_glGetShaderiv = void (GL_APIENTRY*)(GLuint shader, GLenum pname, GLint* params);
extern PFN_glGetShaderiv p_glGetShaderiv;
using PFN_glGetString = const GLubyte* (GL_APIENTRY*)(GLenum name);
extern PFN_glGetString p_glGetString;
using PFN_glGetUniformLocation = GLint (GL_APIENTRY*)(GLuint program, const GLchar* name);
extern PFN_glGetUniformLocation p_glGetUniformLocation;
using PFN_glLineWidth = void (GL_APIENTRY*)(GLfloat width);
extern PFN_glLineWidth p_glLineWidth;
using PFN_glLinkProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glLinkProgram p_glLinkProgram;
using PFN_glScissor = void (GL_APIENTRY*)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFN_glScissor p_glScissor;
using PFN_glShaderSource = void (GL_APIENTRY*)(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
extern PFN_glShaderSource p_glShaderSource;
using PFN_glTexImage2D = void (GL_APIENTRY*)(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels);
extern PFN_glTexImage2D p_glTexImage2D;
using PFN_glTexParameteri = void (GL_APIENTRY*)(GLenum target, GLenum pname, GLint param);
extern PFN_glTexParameteri p_glTexParameteri;
using PFN_glUniform1f = void (GL_APIENTRY*)(GLint location, GLfloat v0);
extern PFN_glUniform1f p_glUniform1f;
using PFN_glUniform1i = void (GL_APIENTRY*)(GLint location, GLint v0);
extern PFN_glUniform1i p_glUniform1i;
using PFN_glUniform2f = void (GL_APIENTRY*)(GLint location, GLfloat v0, GLfloat v1);
extern PFN_glUniform2f p_glUniform2f;
using PFN_glUseProgram = void (GL_APIENTRY*)(GLuint program);
extern PFN_glUseProgram p_glUseProgram;
using PFN_glVertexAttribPointer = void (GL_APIENTRY*)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
extern PFN_glVertexAttribPointer p_glVertexAttribPointer;
using PFN_glViewport = void (GL_APIENTRY*)(GLint x, GLint y, GLsizei width, GLsizei height);
extern PFN_glViewport p_glViewport;
using PFN_glGenVertexArrays = void (GL_APIENTRY*)(GLsizei n, GLuint* arrays);
extern PFN_glGenVertexArrays p_glGenVertexArrays;
using PFN_glBindVertexArray = void (GL_APIENTRY*)(GLuint array);
extern PFN_glBindVertexArray p_glBindVertexArray;
using PFN_glDeleteVertexArrays = void (GL_APIENTRY*)(GLsizei n, const GLuint* arrays);
extern PFN_glDeleteVertexArrays p_glDeleteVertexArrays;

} // namespace gl_api
} // namespace construo

// Map call sites to loaded pointers (not applied while defining the pointers).
#ifndef CONSTRUO_GL_API_IMPLEMENTATION
#  define glActiveTexture ::construo::gl_api::p_glActiveTexture
#  define glAttachShader ::construo::gl_api::p_glAttachShader
#  define glBindBuffer ::construo::gl_api::p_glBindBuffer
#  define glBindTexture ::construo::gl_api::p_glBindTexture
#  define glBlendFunc ::construo::gl_api::p_glBlendFunc
#  define glBufferData ::construo::gl_api::p_glBufferData
#  define glClear ::construo::gl_api::p_glClear
#  define glClearColor ::construo::gl_api::p_glClearColor
#  define glCompileShader ::construo::gl_api::p_glCompileShader
#  define glCreateProgram ::construo::gl_api::p_glCreateProgram
#  define glCreateShader ::construo::gl_api::p_glCreateShader
#  define glDeleteBuffers ::construo::gl_api::p_glDeleteBuffers
#  define glDeleteProgram ::construo::gl_api::p_glDeleteProgram
#  define glDeleteShader ::construo::gl_api::p_glDeleteShader
#  define glDeleteTextures ::construo::gl_api::p_glDeleteTextures
#  define glDisable ::construo::gl_api::p_glDisable
#  define glDisableVertexAttribArray ::construo::gl_api::p_glDisableVertexAttribArray
#  define glDrawArrays ::construo::gl_api::p_glDrawArrays
#  define glEnable ::construo::gl_api::p_glEnable
#  define glEnableVertexAttribArray ::construo::gl_api::p_glEnableVertexAttribArray
#  define glGenBuffers ::construo::gl_api::p_glGenBuffers
#  define glGenTextures ::construo::gl_api::p_glGenTextures
#  define glGetAttribLocation ::construo::gl_api::p_glGetAttribLocation
#  define glGetProgramInfoLog ::construo::gl_api::p_glGetProgramInfoLog
#  define glGetProgramiv ::construo::gl_api::p_glGetProgramiv
#  define glGetShaderInfoLog ::construo::gl_api::p_glGetShaderInfoLog
#  define glGetShaderiv ::construo::gl_api::p_glGetShaderiv
#  define glGetString ::construo::gl_api::p_glGetString
#  define glGetUniformLocation ::construo::gl_api::p_glGetUniformLocation
#  define glLineWidth ::construo::gl_api::p_glLineWidth
#  define glLinkProgram ::construo::gl_api::p_glLinkProgram
#  define glScissor ::construo::gl_api::p_glScissor
#  define glShaderSource ::construo::gl_api::p_glShaderSource
#  define glTexImage2D ::construo::gl_api::p_glTexImage2D
#  define glTexParameteri ::construo::gl_api::p_glTexParameteri
#  define glUniform1f ::construo::gl_api::p_glUniform1f
#  define glUniform1i ::construo::gl_api::p_glUniform1i
#  define glUniform2f ::construo::gl_api::p_glUniform2f
#  define glUseProgram ::construo::gl_api::p_glUseProgram
#  define glVertexAttribPointer ::construo::gl_api::p_glVertexAttribPointer
#  define glViewport ::construo::gl_api::p_glViewport
#  define glGenVertexArrays ::construo::gl_api::p_glGenVertexArrays
#  define glBindVertexArray ::construo::gl_api::p_glBindVertexArray
#  define glDeleteVertexArrays ::construo::gl_api::p_glDeleteVertexArrays
#endif

#else // !_WIN32

#  include <SDL_opengles2.h>

namespace construo {
namespace gl_api {
inline void load() {}
} // namespace gl_api
} // namespace construo

#endif // _WIN32

#endif
