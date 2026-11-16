#include "core/Timer.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

namespace zenit {

float FrameTimer::tick() {
    const double now = glfwGetTime();
    if (last_ < 0.0) last_ = now;
    const double dt = now - last_;
    last_ = now;

    accum_ += dt;
    ++frames_;
    if (accum_ >= 0.5) {
        fps_ = static_cast<float>(frames_ / accum_);
        accum_ = 0.0;
        frames_ = 0;
    }
    return static_cast<float>(dt);
}

} // namespace zenit
