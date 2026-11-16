#pragma once
#include <glm/glm.hpp>

namespace zenit {

// Kula jako ciało sztywne (na razie bez rotacji).
struct RigidBody {
    glm::vec3 position{0.0f};
    glm::vec3 velocity{0.0f};
    float radius      = 0.5f;
    float invMass     = 1.0f;   // 1/masa; 0 = ciało nieruchome
    float restitution = 0.6f;   // sprężystość odbicia (0..1)
};

} // namespace zenit
