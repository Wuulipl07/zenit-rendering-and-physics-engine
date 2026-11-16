#pragma once
#include "renderer/GLLoader.h"

#include <glm/glm.hpp>
#include <cstdint>
#include <vector>

namespace zenit {

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
};

// Siatka trójkątów na GPU (VAO + VBO + EBO). Obiekt tylko do przenoszenia (move-only).
class Mesh {
public:
    Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices);
    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void draw() const;

    // Generatory prymitywów
    static Mesh cube(float halfSize = 0.5f);
    static Mesh sphere(int stacks = 24, int slices = 32);   // promień 1
    static Mesh plane(float halfSize);                      // płaszczyzna y = 0, normalna w górę

private:
    void release();

    GLuint  vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLsizei indexCount_ = 0;
};

} // namespace zenit
