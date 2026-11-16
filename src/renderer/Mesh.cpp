#include "renderer/Mesh.h"

#include <cmath>
#include <cstddef>
#include <utility>

namespace zenit {

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& indices)
    : indexCount_(static_cast<GLsizei>(indices.size())) {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)),
                 vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 static_cast<GLsizeiptr>(indices.size() * sizeof(std::uint32_t)),
                 indices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);   // layout(location = 0) aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));
    glEnableVertexAttribArray(1);   // layout(location = 1) aNormal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));

    glBindVertexArray(0);
}

Mesh::~Mesh() { release(); }

void Mesh::release() {
    if (vao_) glDeleteVertexArrays(1, &vao_);
    if (vbo_) glDeleteBuffers(1, &vbo_);
    if (ebo_) glDeleteBuffers(1, &ebo_);
    vao_ = vbo_ = ebo_ = 0;
    indexCount_ = 0;
}

Mesh::Mesh(Mesh&& o) noexcept
    : vao_(o.vao_), vbo_(o.vbo_), ebo_(o.ebo_), indexCount_(o.indexCount_) {
    o.vao_ = o.vbo_ = o.ebo_ = 0;
    o.indexCount_ = 0;
}

Mesh& Mesh::operator=(Mesh&& o) noexcept {
    if (this != &o) {
        release();
        vao_ = o.vao_; vbo_ = o.vbo_; ebo_ = o.ebo_; indexCount_ = o.indexCount_;
        o.vao_ = o.vbo_ = o.ebo_ = 0;
        o.indexCount_ = 0;
    }
    return *this;
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// ---------------------------------------------------------------- prymitywy

Mesh Mesh::cube(float h) {
    struct Face { glm::vec3 n, u, v; };   // u x v = n  -> wierzchołki CCW patrząc z zewnątrz
    const Face faces[6] = {
        { { 0, 0, 1}, { 1, 0, 0}, {0, 1,  0} },   // +Z
        { { 0, 0,-1}, {-1, 0, 0}, {0, 1,  0} },   // -Z
        { { 1, 0, 0}, { 0, 0,-1}, {0, 1,  0} },   // +X
        { {-1, 0, 0}, { 0, 0, 1}, {0, 1,  0} },   // -X
        { { 0, 1, 0}, { 1, 0, 0}, {0, 0, -1} },   // +Y
        { { 0,-1, 0}, { 1, 0, 0}, {0, 0,  1} },   // -Y
    };

    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;
    for (const Face& f : faces) {
        const std::uint32_t base = static_cast<std::uint32_t>(vertices.size());
        vertices.push_back({ (f.n - f.u - f.v) * h, f.n });
        vertices.push_back({ (f.n + f.u - f.v) * h, f.n });
        vertices.push_back({ (f.n + f.u + f.v) * h, f.n });
        vertices.push_back({ (f.n - f.u + f.v) * h, f.n });
        indices.insert(indices.end(), { base, base + 1, base + 2, base + 2, base + 3, base });
    }
    return Mesh(vertices, indices);
}

Mesh Mesh::sphere(int stacks, int slices) {
    const float pi = 3.14159265358979f;
    std::vector<Vertex> vertices;
    std::vector<std::uint32_t> indices;

    for (int i = 0; i <= stacks; ++i) {
        const float theta = pi * static_cast<float>(i) / static_cast<float>(stacks);
        for (int j = 0; j <= slices; ++j) {
            const float phi = 2.0f * pi * static_cast<float>(j) / static_cast<float>(slices);
            const glm::vec3 p(std::sin(theta) * std::cos(phi),
                              std::cos(theta),
                              std::sin(theta) * std::sin(phi));
            vertices.push_back({ p, p });   // dla sfery jednostkowej normalna = pozycja
        }
    }
    const std::uint32_t row = static_cast<std::uint32_t>(slices + 1);
    for (int i = 0; i < stacks; ++i) {
        for (int j = 0; j < slices; ++j) {
            const std::uint32_t a = static_cast<std::uint32_t>(i) * row + static_cast<std::uint32_t>(j);
            const std::uint32_t b = a + row;
            indices.insert(indices.end(), { a, a + 1, b, a + 1, b + 1, b });
        }
    }
    return Mesh(vertices, indices);
}

Mesh Mesh::plane(float h) {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    std::vector<Vertex> vertices = {
        { {-h, 0.0f, -h}, up },
        { {-h, 0.0f,  h}, up },
        { { h, 0.0f,  h}, up },
        { { h, 0.0f, -h}, up },
    };
    std::vector<std::uint32_t> indices = { 0, 1, 2, 0, 2, 3 };
    return Mesh(vertices, indices);
}

} // namespace zenit
