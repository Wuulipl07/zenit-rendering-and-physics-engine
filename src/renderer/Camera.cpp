#include "renderer/Camera.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <cmath>

namespace zenit {

void Camera::look(float dx, float dy, float sensitivity) {
    yaw   += dx * sensitivity;
    pitch -= dy * sensitivity;
    pitch = std::clamp(pitch, -89.0f, 89.0f);
}

glm::vec3 Camera::forward() const {
    const float y = glm::radians(yaw);
    const float p = glm::radians(pitch);
    return glm::normalize(glm::vec3(std::cos(y) * std::cos(p), std::sin(p), std::sin(y) * std::cos(p)));
}

glm::vec3 Camera::right() const {
    return glm::normalize(glm::cross(forward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::move(float forwardAxis, float rightAxis, float upAxis, float dt) {
    position += forward() * (forwardAxis * moveSpeed * dt);
    position += right()   * (rightAxis   * moveSpeed * dt);
    position += glm::vec3(0.0f, 1.0f, 0.0f) * (upAxis * moveSpeed * dt);
}

glm::mat4 Camera::view() const {
    return glm::lookAt(position, position + forward(), glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 Camera::projection(float aspect) const {
    return glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
}

} // namespace zenit
