#include "core/Input.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <algorithm>
#include <iterator>

namespace zenit {

namespace {

bool g_keys[GLFW_KEY_LAST + 1]{};
bool g_keysPressed[GLFW_KEY_LAST + 1]{};
bool g_buttons[GLFW_MOUSE_BUTTON_LAST + 1]{};
bool g_buttonsPressed[GLFW_MOUSE_BUTTON_LAST + 1]{};

double g_lastX = 0.0, g_lastY = 0.0;
bool   g_hasLast = false;
glm::vec2 g_delta{0.0f};
float  g_scroll = 0.0f;

void onKey(GLFWwindow*, int key, int, int action, int) {
    if (key < 0 || key > GLFW_KEY_LAST) return;
    if (action == GLFW_PRESS) {
        g_keys[key] = true;
        g_keysPressed[key] = true;
    } else if (action == GLFW_RELEASE) {
        g_keys[key] = false;
    }
}

void onMouseButton(GLFWwindow*, int button, int action, int) {
    if (button < 0 || button > GLFW_MOUSE_BUTTON_LAST) return;
    if (action == GLFW_PRESS) {
        g_buttons[button] = true;
        g_buttonsPressed[button] = true;
    } else if (action == GLFW_RELEASE) {
        g_buttons[button] = false;
    }
}

void onCursor(GLFWwindow*, double x, double y) {
    if (g_hasLast) {
        g_delta.x += static_cast<float>(x - g_lastX);
        g_delta.y += static_cast<float>(y - g_lastY);
    }
    g_lastX = x;
    g_lastY = y;
    g_hasLast = true;
}

void onScroll(GLFWwindow*, double, double yoffset) {
    g_scroll += static_cast<float>(yoffset);
}

} // namespace

void Input::attach(GLFWwindow* window) {
    glfwSetKeyCallback(window, onKey);
    glfwSetMouseButtonCallback(window, onMouseButton);
    glfwSetCursorPosCallback(window, onCursor);
    glfwSetScrollCallback(window, onScroll);
}

void Input::beginFrame() {
    std::fill(std::begin(g_keysPressed), std::end(g_keysPressed), false);
    std::fill(std::begin(g_buttonsPressed), std::end(g_buttonsPressed), false);
    g_delta = glm::vec2(0.0f);
    g_scroll = 0.0f;
}

void Input::resetMouse() { g_hasLast = false; g_delta = glm::vec2(0.0f); }

bool Input::down(int key)       { return key >= 0 && key <= GLFW_KEY_LAST && g_keys[key]; }
bool Input::pressed(int key)    { return key >= 0 && key <= GLFW_KEY_LAST && g_keysPressed[key]; }
bool Input::mouseDown(int b)    { return b >= 0 && b <= GLFW_MOUSE_BUTTON_LAST && g_buttons[b]; }
bool Input::mousePressed(int b) { return b >= 0 && b <= GLFW_MOUSE_BUTTON_LAST && g_buttonsPressed[b]; }
glm::vec2 Input::mouseDelta()   { return g_delta; }
float Input::scroll()           { return g_scroll; }

} // namespace zenit
