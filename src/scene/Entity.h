#pragma once
#include "renderer/Mesh.h"

#include <glm/glm.hpp>

namespace zenit {

// Obiekt sceny = co rysować (mesh) + gdzie + jak + (opcjonalnie) które ciało fizyki go napędza.
struct Entity {
    const Mesh* mesh = nullptr;       // nie jest właścicielem; mesh trzyma Scene
    glm::vec3 position{0.0f};
    glm::vec3 scale{1.0f};
    glm::vec3 color{1.0f};
    float angle = 0.0f;               // aktualny kąt obrotu (rad)
    float spin  = 0.0f;               // prędkość obrotu (rad/s); 0 = brak
    int   body  = -1;                 // indeks w PhysicsWorld; -1 = bez fizyki
    bool  checker = false;            // szachownica (podłoga)
};

} // namespace zenit
