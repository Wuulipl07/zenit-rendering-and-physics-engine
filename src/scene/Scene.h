#pragma once
#include "physics/PhysicsWorld.h"
#include "renderer/Mesh.h"
#include "renderer/Renderer.h"
#include "scene/Entity.h"

#include <glm/glm.hpp>
#include <cstddef>
#include <random>
#include <vector>

namespace zenit {

// Scena demo: podłoga z szachownicą, obracający się czerwony sześcian (hołd dla pierwszej
// wersji) i kule spadające według fizyki. Scena łączy renderer z fizyką.
class Scene {
public:
    Scene();
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;

    void update(float dt);              // fizyka (stały krok) + animacje
    void render(Renderer& renderer);

    void spawnSphere(const glm::vec3& position, const glm::vec3& velocity);
    void spawnBurst(int count);         // kule z góry w losowych miejscach
    void reset();                       // usuń kule i zrzuć nową partię

    std::size_t sphereCount() const { return physics_.size(); }

private:
    float rand(float lo, float hi);

    Mesh cube_;
    Mesh sphere_;
    Mesh plane_;
    PhysicsWorld physics_;
    std::vector<Entity> entities_;

    float accumulator_ = 0.0f;
    std::mt19937 rng_;
};

} // namespace zenit
