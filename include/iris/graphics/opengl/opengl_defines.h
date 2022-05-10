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
#define GL_DYNAMIC_DRAW 0x88E8
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
#define GL_READ_FRAMEBUFFER 0x8CA8
#define GL_MIRRORED_REPEAT 0x8370
#define GL_CLAMP_TO_BORDER 0x812D
#define GL_CLAMP_TO_EDGE 0x812F
#define GL_RGBA16F 0x881A
#define GL_SRGB 0x8C40
#define GL_SRGB_ALPHA 0x8C42
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#define GL_TEXTURE_WRAP_R 0x8072
#define GL_TEXTURE_CUBE_MAP 0x8513
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
#define GL_UNIFORM_BUFFER 0x8A11
#define GL_COPY_READ_BUFFER 0x8F36
#define GL_COPY_WRITE_BUFFER 0x8F37
#define GL_SHADER_STORAGE_BUFFER 0x90D2

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

// platform specific defines
#if defined(IRIS_PLATFORM_WIN32)
#define GL_COLOR_ATTACHMENT1 0x8CE1
#define GL_COLOR_ATTACHMENT2 0x8CE2
#endif

using GLsizeiptr = std::ptrdiff_t;
using GLintptr = std::ptrdiff_t;
using GLchar = char;
using GLuint64 = std::uint64_t;

// x-macro definition for all opengl functions we want to laod
// by default when we include opengl.h we want all these to be marked extern we will then define them all once in a
// single translation unit where they can be resolved
// read comments in opengl.h for more details on the EXTERN macro

// platform specific functions to resolve
#if defined(IRIS_PLATFORM_WIN32)
#define FOR_OPENGL_FUNCTIONS(DO)                                                                                       \
    DO(void, glDeleteBuffers, GLsizei, const GLuint *)                                                                 \
    DO(void, glUseProgram, GLuint)                                                                                     \
    DO(void, glBindBuffer, GLenum, GLuint)                                                                             \
    DO(void, glGenVertexArrays, GLsizei, GLuint *)                                                                     \
    DO(void, glDeleteVertexArrays, GLsizei, GLuint *)                                                                  \
    DO(void, glBindVertexArray, GLuint)                                                                                \
    DO(void, glEnableVertexAttribArray, GLuint)                                                                        \
    DO(void, glVertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)                           \
    DO(void, glVertexAttribIPointer, GLuint, GLint, GLenum, GLsizei, const void *)                                     \
    DO(GLuint, glCreateProgram, void)                                                                                  \
    DO(void, glAttachShader, GLuint, GLuint)                                                                           \
    DO(void, glGenBuffers, GLsizei, GLuint *)                                                                          \
    DO(void, glBufferData, GLenum, GLsizeiptr, const void *, GLenum)                                                   \
    DO(void, glBufferSubData, GLenum, GLintptr, GLsizeiptr, const void *)                                              \
    DO(void, glLinkProgram, GLuint)                                                                                    \
    DO(void, glGetProgramiv, GLuint, GLenum, GLint *)                                                                  \
    DO(void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei *, GLchar *)                                                \
    DO(void, glDeleteProgram, GLuint)                                                                                  \
    DO(void, glGenFramebuffers, GLsizei, GLuint *)                                                                     \
    DO(void, glBindFramebuffer, GLenum, GLuint)                                                                        \
    DO(void, glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint)                                            \
    DO(GLenum, glCheckFramebufferStatus, GLenum)                                                                       \
    DO(void, glDeleteFramebuffers, GLsizei, const GLuint *)                                                            \
    DO(GLint, glGetUniformLocation, GLuint, const GLchar *)                                                            \
    DO(void, glUniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat *)                                           \
    DO(void, glUniform3f, GLint, GLfloat, GLfloat, GLfloat)                                                            \
    DO(void, glUniform1fv, GLint, GLsizei, const GLfloat *)                                                            \
    DO(void, glUniform4fv, GLint, GLsizei, const GLfloat *)                                                            \
    DO(void, glUniform1i, GLint, GLint)                                                                                \
    DO(void, glBlitFramebuffer, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)            \
    DO(GLuint, glCreateShader, GLenum)                                                                                 \
    DO(void, glShaderSource, GLuint, GLsizei, const GLchar **, const GLint *)                                          \
    DO(void, glCompileShader, GLuint)                                                                                  \
    DO(void, glGetShaderiv, GLuint, GLenum, GLint *)                                                                   \
    DO(void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei *, GLchar *)                                                 \
    DO(void, glDeleteShader, GLuint)                                                                                   \
    DO(void, glGenerateMipmap, GLenum)                                                                                 \
    DO(void, glBindBufferBase, GLenum, GLuint, GLuint)                                                                 \
    DO(void, glBindBufferRange, GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)                                          \
    DO(GLuint64, glGetTextureHandleARB, GLuint)                                                                        \
    DO(GLuint64, glGetTextureSamplerHandleARB, GLuint, GLuint)                                                         \
    DO(void, glMakeTextureHandleResidentARB, GLuint64)                                                                 \
    DO(void, glMakeTextureHandleNonResidentARB, GLuint64)                                                              \
    DO(void, glCopyBufferSubData, GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)                                      \
    DO(void, glDrawElementsInstanced, GLenum, GLsizei, GLenum, const void *, GLsizei)                                  \
    DO(void, glGenSamplers, GLsizei, GLuint *)                                                                         \
    DO(void, glBindSampler, GLuint, GLuint)                                                                            \
    DO(void, glDeleteSamplers, GLsizei, const GLuint *)                                                                \
    DO(void, glSamplerParameteri, GLuint, GLenum, GLint)                                                               \
    DO(void, glSamplerParameterfv, GLuint, GLenum, const GLfloat *)                                                    \
    DO(void, glDrawBuffers, GLsizei, const GLenum *)                                                                   \
    DO(void, glActiveTexture, GLenum)
#elif defined(IRIS_PLATFORM_LINUX)
#define FOR_OPENGL_FUNCTIONS(DO)                                                                                       \
    DO(void, glDeleteBuffers, GLsizei, const GLuint *)                                                                 \
    DO(void, glUseProgram, GLuint)                                                                                     \
    DO(void, glBindBuffer, GLenum, GLuint)                                                                             \
    DO(void, glGenVertexArrays, GLsizei, GLuint *)                                                                     \
    DO(void, glDeleteVertexArrays, GLsizei, GLuint *)                                                                  \
    DO(void, glBindVertexArray, GLuint)                                                                                \
    DO(void, glEnableVertexAttribArray, GLuint)                                                                        \
    DO(void, glVertexAttribPointer, GLuint, GLint, GLenum, GLboolean, GLsizei, const void *)                           \
    DO(void, glVertexAttribIPointer, GLuint, GLint, GLenum, GLsizei, const void *)                                     \
    DO(GLuint, glCreateProgram, void)                                                                                  \
    DO(void, glAttachShader, GLuint, GLuint)                                                                           \
    DO(void, glGenBuffers, GLsizei, GLuint *)                                                                          \
    DO(void, glBufferData, GLenum, GLsizeiptr, const void *, GLenum)                                                   \
    DO(void, glBufferSubData, GLenum, GLintptr, GLsizeiptr, const void *)                                              \
    DO(void, glLinkProgram, GLuint)                                                                                    \
    DO(void, glGetProgramiv, GLuint, GLenum, GLint *)                                                                  \
    DO(void, glGetProgramInfoLog, GLuint, GLsizei, GLsizei *, GLchar *)                                                \
    DO(void, glDeleteProgram, GLuint)                                                                                  \
    DO(void, glGenFramebuffers, GLsizei, GLuint *)                                                                     \
    DO(void, glBindFramebuffer, GLenum, GLuint)                                                                        \
    DO(void, glFramebufferTexture2D, GLenum, GLenum, GLenum, GLuint, GLint)                                            \
    DO(GLenum, glCheckFramebufferStatus, GLenum)                                                                       \
    DO(void, glDeleteFramebuffers, GLsizei, const GLuint *)                                                            \
    DO(GLint, glGetUniformLocation, GLuint, const GLchar *)                                                            \
    DO(void, glUniformMatrix4fv, GLint, GLsizei, GLboolean, const GLfloat *)                                           \
    DO(void, glUniform3f, GLint, GLfloat, GLfloat, GLfloat)                                                            \
    DO(void, glUniform1fv, GLint, GLsizei, const GLfloat *)                                                            \
    DO(void, glUniform4fv, GLint, GLsizei, const GLfloat *)                                                            \
    DO(void, glUniform1i, GLint, GLint)                                                                                \
    DO(void, glBlitFramebuffer, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLint, GLbitfield, GLenum)            \
    DO(GLuint, glCreateShader, GLenum)                                                                                 \
    DO(void, glShaderSource, GLuint, GLsizei, const GLchar **, const GLint *)                                          \
    DO(void, glCompileShader, GLuint)                                                                                  \
    DO(void, glGetShaderiv, GLuint, GLenum, GLint *)                                                                   \
    DO(void, glGetShaderInfoLog, GLuint, GLsizei, GLsizei *, GLchar *)                                                 \
    DO(void, glDeleteShader, GLuint)                                                                                   \
    DO(void, glGenerateMipmap, GLenum)                                                                                 \
    DO(void, glBindBufferBase, GLenum, GLuint, GLuint)                                                                 \
    DO(void, glBindBufferRange, GLenum, GLuint, GLuint, GLintptr, GLsizeiptr)                                          \
    DO(GLuint64, glGetTextureHandleARB, GLuint)                                                                        \
    DO(GLuint64, glGetTextureSamplerHandleARB, GLuint, GLuint)                                                         \
    DO(void, glMakeTextureHandleResidentARB, GLuint64)                                                                 \
    DO(void, glMakeTextureHandleNonResidentARB, GLuint64)                                                              \
    DO(void, glCopyBufferSubData, GLenum, GLenum, GLintptr, GLintptr, GLsizeiptr)                                      \
    DO(void, glDrawElementsInstanced, GLenum, GLsizei, GLenum, const void *, GLsizei)                                  \
    DO(void, glGenSamplers, GLsizei, GLuint *)                                                                         \
    DO(void, glBindSampler, GLuint, GLuint)                                                                            \
    DO(void, glDeleteSamplers, GLsizei, const GLuint *)                                                                \
    DO(void, glSamplerParameteri, GLuint, GLenum, GLint)                                                               \
    DO(void, glSamplerParameterfv, GLuint, GLenum, const GLfloat *)                                                    \
    DO(void, glDrawBuffers, GLsizei, const GLenum *)
#endif

// declare all functions
#define DECLARE_FUNCTIONS(RETURN, NAME, ...) EXTERN RETURN (*NAME)(__VA_ARGS__);
FOR_OPENGL_FUNCTIONS(DECLARE_FUNCTIONS)
