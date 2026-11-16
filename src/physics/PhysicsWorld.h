#pragma once
#include "physics/RigidBody.h"

#include <glm/glm.hpp>
#include <cstddef>
#include <vector>

namespace zenit {

// Prosty świat fizyki: grawitacja, podłoga (y = groundY), niewidzialne ściany areny
// i kolizje kula-kula (impuls + korekcja pozycji). Całkowanie: semi-implicit Euler.
//
// Ten moduł NIE zależy od OpenGL ani od renderera - można go testować osobno.
class PhysicsWorld {
public:
    struct Config {
        glm::vec3 gravity{0.0f, -9.81f, 0.0f};
        float groundY          = 0.0f;
        float arenaHalfExtent  = 12.0f;   // ściany w x,z = +/- ta wartość
        float groundFriction   = 1.2f;    // tłumienie ruchu po podłodze (1/s)
        float wallRestitution  = 0.5f;
    };

    Config config;

    // Zwraca indeks ciała (stabilny do czasu clear()).
    int addSphere(const glm::vec3& position, const glm::vec3& velocity,
                  float radius, float mass, float restitution = 0.6f);

    void step(float dt);      // wołaj ze STAŁYM krokiem (np. 1/120 s)
    void clear();

    const RigidBody& body(int index) const { return bodies_[static_cast<std::size_t>(index)]; }
    std::size_t size() const { return bodies_.size(); }

private:
    void collideSpheres();
    void collideBounds(float dt);

    std::vector<RigidBody> bodies_;
};

} // namespace zenit
