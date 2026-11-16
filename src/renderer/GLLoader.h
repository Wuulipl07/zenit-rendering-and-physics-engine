#pragma once
// Ładowanie funkcji OpenGL przez glfwGetProcAddress (bez GLEW - działa na X11 i Wayland).
// Chcesz użyć nowej funkcji GL? Dopisz jedną linię X(PFN...PROC, nazwa) do listy poniżej.
#include <GL/glcorearb.h>

#define ZENIT_GL_FUNCS(X)                                                  \
    X(PFNGLCREATESHADERPROC,            glCreateShader)                    \
    X(PFNGLSHADERSOURCEPROC,            glShaderSource)                    \
    X(PFNGLCOMPILESHADERPROC,           glCompileShader)                   \
    X(PFNGLGETSHADERIVPROC,             glGetShaderiv)                     \
    X(PFNGLGETSHADERINFOLOGPROC,        glGetShaderInfoLog)                \
    X(PFNGLDELETESHADERPROC,            glDeleteShader)                    \
    X(PFNGLCREATEPROGRAMPROC,           glCreateProgram)                   \
    X(PFNGLATTACHSHADERPROC,            glAttachShader)                    \
    X(PFNGLLINKPROGRAMPROC,             glLinkProgram)                     \
    X(PFNGLGETPROGRAMIVPROC,            glGetProgramiv)                    \
    X(PFNGLGETPROGRAMINFOLOGPROC,       glGetProgramInfoLog)               \
    X(PFNGLDELETEPROGRAMPROC,           glDeleteProgram)                   \
    X(PFNGLUSEPROGRAMPROC,              glUseProgram)                      \
    X(PFNGLGETUNIFORMLOCATIONPROC,      glGetUniformLocation)              \
    X(PFNGLUNIFORM1IPROC,               glUniform1i)                       \
    X(PFNGLUNIFORM1FPROC,               glUniform1f)                       \
    X(PFNGLUNIFORM3FPROC,               glUniform3f)                       \
    X(PFNGLUNIFORMMATRIX4FVPROC,        glUniformMatrix4fv)                \
    X(PFNGLGENVERTEXARRAYSPROC,         glGenVertexArrays)                 \
    X(PFNGLBINDVERTEXARRAYPROC,         glBindVertexArray)                 \
    X(PFNGLDELETEVERTEXARRAYSPROC,      glDeleteVertexArrays)              \
    X(PFNGLGENBUFFERSPROC,              glGenBuffers)                      \
    X(PFNGLBINDBUFFERPROC,              glBindBuffer)                      \
    X(PFNGLBUFFERDATAPROC,              glBufferData)                      \
    X(PFNGLDELETEBUFFERSPROC,           glDeleteBuffers)                   \
    X(PFNGLVERTEXATTRIBPOINTERPROC,     glVertexAttribPointer)             \
    X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)         \
    X(PFNGLVIEWPORTPROC,                glViewport)                        \
    X(PFNGLGETSTRINGPROC,               glGetString)                       \
    X(PFNGLENABLEPROC,                  glEnable)                          \
    X(PFNGLDISABLEPROC,                 glDisable)                         \
    X(PFNGLCLEARCOLORPROC,              glClearColor)                      \
    X(PFNGLCLEARPROC,                   glClear)                           \
    X(PFNGLPOLYGONMODEPROC,             glPolygonMode)                     \
    X(PFNGLDRAWELEMENTSPROC,            glDrawElements)

#define ZENIT_GL_DECLARE(type, name) extern type name;
ZENIT_GL_FUNCS(ZENIT_GL_DECLARE)
#undef ZENIT_GL_DECLARE

// Wołać po glfwMakeContextCurrent. Zwraca false, jeśli którejś funkcji brakuje.
bool loadGL();
