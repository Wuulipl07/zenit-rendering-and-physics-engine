#include "scene/Scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace zenit {

namespace {
constexpr float       kPhysicsStep = 1.0f / 120.0f;
constexpr std::size_t kMaxSpheres  = 300;
}

Scene::Scene()
    : cube_(Mesh::cube(0.5f)),
      sphere_(Mesh::sphere()),
      plane_(Mesh::plane(12.0f)),     // = physics_.config.arenaHalfExtent
      rng_(std::random_device{}()) {

    // podłoga
    Entity ground;
    ground.mesh    = &plane_;
    ground.color   = glm::vec3(0.75f, 0.80f, 0.75f);
    ground.checker = true;
    entities_.push_back(ground);

    // dekoracyjny, obracający się czerwony sześcian (bez fizyki)
    Entity cube;
    cube.mesh     = &cube_;
    cube.position = glm::vec3(0.0f, 3.0f, -6.0f);
    cube.scale    = glm::vec3(2.0f);
    cube.color    = glm::vec3(0.9f, 0.05f, 0.05f);
    cube.spin     = 0.8f;
    entities_.push_back(cube);

    spawnBurst(20);
}

float Scene::rand(float lo, float hi) {
    return std::uniform_real_distribution<float>(lo, hi)(rng_);
}

void Scene::spawnSphere(const glm::vec3& position, const glm::vec3& velocity) {
    if (physics_.size() >= kMaxSpheres) return;

    const float radius = rand(0.3f, 0.7f);
    const float mass   = radius * radius * radius * 4.0f;   // masa ~ objętość

    Entity e;
    e.mesh  = &sphere_;
    e.scale = glm::vec3(radius);
    e.color = glm::vec3(rand(0.3f, 1.0f), rand(0.3f, 1.0f), rand(0.3f, 1.0f));
    e.body  = physics_.addSphere(position, velocity, radius, mass, 0.6f);
    e.position = position;
    entities_.push_back(e);
}

void Scene::spawnBurst(int count) {
    for (int i = 0; i < count; ++i) {
        spawnSphere(glm::vec3(rand(-4.0f, 4.0f), rand(6.0f, 18.0f), rand(-4.0f, 4.0f)),
                    glm::vec3(rand(-1.0f, 1.0f), 0.0f, rand(-1.0f, 1.0f)));
    }
}

void Scene::reset() {
    entities_.erase(std::remove_if(entities_.begin(), entities_.end(),
                                   [](const Entity& e) { return e.body >= 0; }),
                    entities_.end());
    physics_.clear();
    accumulator_ = 0.0f;
    spawnBurst(20);
}

void Scene::update(float dt) {
    accumulator_ += std::min(dt, 0.1f);          // limit przy dużych przycięciach
    while (accumulator_ >= kPhysicsStep) {
        physics_.step(kPhysicsStep);
        accumulator_ -= kPhysicsStep;
    }

    for (Entity& e : entities_) {
        if (e.body >= 0) e.position = physics_.body(e.body).position;
        e.angle += e.spin * dt;
    }
}

void Scene::render(Renderer& renderer) {
    for (const Entity& e : entities_) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, e.position);
        if (e.spin != 0.0f) {
            model = glm::rotate(model, e.angle,        glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, e.angle * 0.6f, glm::vec3(1.0f, 0.0f, 0.0f));
        }
        model = glm::scale(model, e.scale);
        renderer.draw(*e.mesh, model, e.color, e.checker);
    }
}

} // namespace zenit
