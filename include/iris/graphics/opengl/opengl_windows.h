////////////////////////////////////////////////////////////////////////////////
//         Distributed under the Boost Software License, Version 1.0.         //
//            (See accompanying file LICENSE or copy at                       //
//                 https://www.boost.org/LICENSE_1_0.txt)                     //
////////////////////////////////////////////////////////////////////////////////

/**
 * This is an incomplete file and is intended to be included in
 * include/opengl/opengl.h
 *
 * *DO NOT* include this file directly
 *
 * This file defines various opengl constants and functions required for windows
 */

#define GL_ARRAY_BUFFER 0x8892
#define GL_ELEMENT_ARRAY_BUFFER 0x8893
#define GL_STATIC_DRAW 0x88E4
#define GL_LINK_STATUS 0x8B82
#define GL_INFO_LOG_LENGTH 0x8B84
#define GL_FRAMEBUFFER 0x8D40
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_DEPTH_ATTACHMENT 0x8D00
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_TEXTURE0 0x84C0
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_DRAW_FRAMEBUFFER 0x8CA9
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31
#define GL_COMPILE_STATUS 0x8B81
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_RGBA16F 0x881A
#define GL_SRGB 0x8C40
#define GL_SRGB_ALPHA 0x8C42
#define GL_FRAMEBUFFER_SRGB 0x8DB9

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_ACCELERATION_ARB 0x2003
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023
#define WGL_FULL_ACCELERATION_ARB 0x2027
#define WGL_TYPE_RGBA_ARB 0x202B

using GLsizeiptr = std::ptrdiff_t;
using GLintptr = std::ptrdiff_t;
using GLchar = char;

// opengl function pointers
// because windows supports multiple implementations of the opengl functions we
// will need to resolve all these
// by default when we include opengl.h we want all these to be marked extern
// we will then define them all once in a single translation unit where they can
// be resolved
// read comments in opengl.h for more details on the EXTERN macro

EXTERN void (*glDeleteBuffers)(GLsizei, const GLuint *);
EXTERN void (*glUseProgram)(GLuint);
EXTERN void (*glBindBuffer)(GLenum, GLuint);
EXTERN void (*glGenVertexArrays)(GLsizei, GLuint *);
EXTERN void (*glDeleteVertexArrays)(GLsizei, GLuint *);
EXTERN void (*glBindVertexArray)(GLuint);
EXTERN void (*glEnableVertexAttribArray)(GLuint);
EXTERN void (*glVertexAttribPointer)(
    GLuint,
    GLint,
    GLenum,
    GLboolean,
    GLsizei,
    const void *);
EXTERN void (
    *glVertexAttribIPointer)(GLuint, GLint, GLenum, GLsizei, const void *);
EXTERN GLuint (*glCreateProgram)(void);
EXTERN void (*glAttachShader)(GLuint, GLuint);
EXTERN void (*glGenBuffers)(GLsizei, GLuint *);
EXTERN void (*glBufferData)(GLenum, GLsizeiptr, const void *, GLenum);
EXTERN void (*glBufferSubData)(GLenum, GLintptr, GLsizeiptr, const void *);
EXTERN void (*glLinkProgram)(GLuint);
EXTERN void (*glGetProgramiv)(GLuint, GLenum, GLint *);
EXTERN void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
EXTERN void (*glDeleteProgram)(GLuint);
EXTERN void (*glGenFramebuffers)(GLsizei, GLuint *);
EXTERN void (*glBindFramebuffer)(GLenum, GLuint);
EXTERN void (*glFramebufferTexture2D)(GLenum, GLenum, GLenum, GLuint, GLint);
EXTERN GLenum (*glCheckFramebufferStatus)(GLenum);
EXTERN void (*glDeleteFramebuffers)(GLsizei, const GLuint *);
EXTERN GLint (*glGetUniformLocation)(GLuint, const GLchar *);
EXTERN void (*glUniformMatrix4fv)(GLint, GLsizei, GLboolean, const GLfloat *);
EXTERN void (*glUniform3f)(GLint, GLfloat, GLfloat, GLfloat);
EXTERN void (*glUniform1fv)(GLint, GLsizei, const GLfloat *);
EXTERN void (*glUniform4fv)(GLint, GLsizei, const GLfloat *);
EXTERN void (*glActiveTexture)(GLenum);
EXTERN void (*glUniform1i)(GLint, GLint);
EXTERN void (*glBlitFramebuffer)(
    GLint,
    GLint,
    GLint,
    GLint,
    GLint,
    GLint,
    GLint,
    GLint,
    GLbitfield,
    GLenum);
EXTERN GLuint (*glCreateShader)(GLenum);
EXTERN void (*glShaderSource)(GLuint, GLsizei, const GLchar **, const GLint *);
EXTERN void (*glCompileShader)(GLuint);
EXTERN void (*glGetShaderiv)(GLuint, GLenum, GLint *);
EXTERN void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei *, GLchar *);
EXTERN void (*glDeleteShader)(GLuint);
EXTERN void (*glGenerateMipmap)(GLenum);