#include "physics/PhysicsWorld.h"

#include <algorithm>
#include <cmath>

namespace zenit {

namespace {
constexpr float kRestVelocity   = 0.6f;   // poniżej tej prędkości odbicia od podłogi wygasają
constexpr float kRestingContact = 0.5f;   // poniżej tej prędkości zderzenia kul są nieelastyczne
}

int PhysicsWorld::addSphere(const glm::vec3& position, const glm::vec3& velocity,
                            float radius, float mass, float restitution) {
    RigidBody b;
    b.position    = position;
    b.velocity    = velocity;
    b.radius      = radius;
    b.invMass     = mass > 0.0f ? 1.0f / mass : 0.0f;
    b.restitution = restitution;
    bodies_.push_back(b);
    return static_cast<int>(bodies_.size()) - 1;
}

void PhysicsWorld::clear() { bodies_.clear(); }

void PhysicsWorld::step(float dt) {
    for (RigidBody& b : bodies_) {
        if (b.invMass == 0.0f) continue;
        b.velocity += config.gravity * dt;
        b.position += b.velocity * dt;
    }
    collideSpheres();
    collideBounds(dt);   // ściany/podłoga mają ostatnie słowo
}

void PhysicsWorld::collideSpheres() {
    const std::size_t n = bodies_.size();
    for (std::size_t i = 0; i < n; ++i) {
        for (std::size_t j = i + 1; j < n; ++j) {
            RigidBody& a = bodies_[i];
            RigidBody& b = bodies_[j];

            const float wSum = a.invMass + b.invMass;
            if (wSum == 0.0f) continue;

            const glm::vec3 d = b.position - a.position;
            const float r = a.radius + b.radius;
            const float dist2 = glm::dot(d, d);
            if (dist2 >= r * r) continue;

            const float dist = std::sqrt(dist2);
            const glm::vec3 n_ = dist > 1e-6f ? d / dist : glm::vec3(0.0f, 1.0f, 0.0f);

            // 1) rozsuń kule proporcjonalnie do odwrotności mas
            const float penetration = r - dist;
            a.position -= n_ * (penetration * a.invMass / wSum);
            b.position += n_ * (penetration * b.invMass / wSum);

            // 2) impuls wzdłuż normalnej
            const float vn = glm::dot(b.velocity - a.velocity, n_);
            if (vn > 0.0f) continue;   // już się oddalają

            float e = std::min(a.restitution, b.restitution);
            if (-vn < kRestingContact) e = 0.0f;   // mniej drgań w stosach

            const float impulse = -(1.0f + e) * vn / wSum;
            a.velocity -= n_ * (impulse * a.invMass);
            b.velocity += n_ * (impulse * b.invMass);
        }
    }
}

void PhysicsWorld::collideBounds(float dt) {
    const float friction = std::max(0.0f, 1.0f - config.groundFriction * dt);

    for (RigidBody& b : bodies_) {
        if (b.invMass == 0.0f) continue;

        // podłoga
        const float minY = config.groundY + b.radius;
        bool onGround = false;
        if (b.position.y < minY) {
            b.position.y = minY;
            if (b.velocity.y < 0.0f) {
                b.velocity.y = -b.velocity.y * b.restitution;
                if (b.velocity.y < kRestVelocity) b.velocity.y = 0.0f;
            }
            onGround = true;
        }
        if (onGround) {
            b.velocity.x *= friction;
            b.velocity.z *= friction;
        }

        // ściany areny
        const float lim = config.arenaHalfExtent - b.radius;
        if (b.position.x > lim)  { b.position.x =  lim; if (b.velocity.x > 0.0f) b.velocity.x *= -config.wallRestitution; }
        if (b.position.x < -lim) { b.position.x = -lim; if (b.velocity.x < 0.0f) b.velocity.x *= -config.wallRestitution; }
        if (b.position.z > lim)  { b.position.z =  lim; if (b.velocity.z > 0.0f) b.velocity.z *= -config.wallRestitution; }
        if (b.position.z < -lim) { b.position.z = -lim; if (b.velocity.z < 0.0f) b.velocity.z *= -config.wallRestitution; }
    }
}

} // namespace zenit
