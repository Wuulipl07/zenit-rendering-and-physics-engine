#pragma once
#include <glm/glm.hpp>

namespace zenit {

// Kamera "free-fly". Nie zna wejścia - sterujesz nią metodami look() i move().
class Camera {
public:
    glm::vec3 position{0.0f, 5.0f, 16.0f};
    float yaw   = -90.0f;     // stopnie; -90 = patrzy w stronę -Z
    float pitch = -15.0f;     // stopnie, ograniczone do +/-89
    float fov   = 60.0f;      // pionowy, stopnie
    float nearPlane = 0.1f;
    float farPlane  = 300.0f;
    float moveSpeed = 8.0f;   // jednostek / s

    void look(float dx, float dy, float sensitivity = 0.1f);          // dx,dy - ruch myszy w pikselach
    void move(float forwardAxis, float rightAxis, float upAxis, float dt);   // osie w zakresie -1..1

    glm::vec3 forward() const;
    glm::vec3 right() const;
    glm::mat4 view() const;
    glm::mat4 projection(float aspect) const;
};

} // namespace zenit
