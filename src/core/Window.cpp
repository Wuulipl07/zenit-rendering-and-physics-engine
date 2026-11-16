#include "core/Window.h"
#include "core/Input.h"
#include "core/Log.h"
#include "renderer/GLLoader.h"     // musi być przed GLFW (GLFW_INCLUDE_NONE)

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace zenit {

namespace {

GLFWwindow* tryCreate(int major, int minor, int w, int h, const char* title) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    return glfwCreateWindow(w, h, title, nullptr, nullptr);
}

void onGlfwError(int code, const char* desc) {
    log::error("GLFW %d: %s", code, desc);
}

void onFramebufferResize(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

} // namespace

Window::Window(int width, int height, const std::string& title) {
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) throw std::runtime_error("Nie udalo sie zainicjalizowac GLFW");

    window_ = tryCreate(4, 6, width, height, title.c_str());
    if (!window_) {
        log::warn("OpenGL 4.6 niedostepny, probuje 3.3...");
        glslVersion_ = "#version 330 core\n";
        window_ = tryCreate(3, 3, width, height, title.c_str());
    }
    if (!window_) {
        glfwTerminate();
        throw std::runtime_error("Nie udalo sie utworzyc okna / kontekstu OpenGL");
    }

    glfwMakeContextCurrent(window_);
    glfwSwapInterval(1);   // vsync

    if (!loadGL()) {
        glfwDestroyWindow(window_);
        glfwTerminate();
        throw std::runtime_error("Nie udalo sie zaladowac funkcji OpenGL");
    }

    glfwSetFramebufferSizeCallback(window_, onFramebufferResize);
    glViewport(0, 0, framebufferWidth(), framebufferHeight());
    Input::attach(window_);

    log::info("GL_VERSION:  %s", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    log::info("GL_RENDERER: %s", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
}

Window::~Window() {
    if (window_) glfwDestroyWindow(window_);
    glfwTerminate();
}

bool Window::shouldClose() const { return glfwWindowShouldClose(window_) != 0; }
void Window::close()             { glfwSetWindowShouldClose(window_, GLFW_TRUE); }
void Window::pollEvents()        { glfwPollEvents(); }
void Window::swapBuffers()       { glfwSwapBuffers(window_); }
void Window::setTitle(const std::string& t) { glfwSetWindowTitle(window_, t.c_str()); }

void Window::setCursorCaptured(bool captured) {
    captured_ = captured;
    glfwSetInputMode(window_, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    Input::resetMouse();
}

int Window::framebufferWidth() const {
    int w = 0, h = 0; glfwGetFramebufferSize(window_, &w, &h); return w;
}
int Window::framebufferHeight() const {
    int w = 0, h = 0; glfwGetFramebufferSize(window_, &w, &h); return h;
}
float Window::aspect() const {
    int w = 0, h = 0; glfwGetFramebufferSize(window_, &w, &h);
    return h > 0 ? static_cast<float>(w) / static_cast<float>(h) : 1.0f;
}

} // namespace zenit
