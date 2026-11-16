#include "renderer/GLLoader.h"
#include "core/Log.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#define ZENIT_GL_DEFINE(type, name) type name = nullptr;
ZENIT_GL_FUNCS(ZENIT_GL_DEFINE)
#undef ZENIT_GL_DEFINE

bool loadGL() {
    bool ok = true;
#define ZENIT_GL_LOAD(type, name)                                              \
    name = reinterpret_cast<type>(glfwGetProcAddress(#name));                  \
    if (!name) { zenit::log::error("Brak funkcji GL: %s", #name); ok = false; }
    ZENIT_GL_FUNCS(ZENIT_GL_LOAD)
#undef ZENIT_GL_LOAD
    return ok;
}
