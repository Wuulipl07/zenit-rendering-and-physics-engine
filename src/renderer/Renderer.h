#pragma once
#include "renderer/Camera.h"
#include "renderer/Mesh.h"
#include "renderer/Shader.h"

#include <glm/glm.hpp>

namespace zenit {

// Rysuje siatki jednym shaderem (shaders/basic.vert + basic.frag).
// Kolejność w klatce: beginFrame() -> draw() x N.
class Renderer {
public:
    explicit Renderer(const char* glslVersion);

    void beginFrame(const Camera& camera, float aspect);
    void draw(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color, bool checker = false);

    void setWireframe(bool on);
    bool wireframe() const { return wireframe_; }
    void setClearColor(const glm::vec3& c) { clearColor_ = c; }

private:
    Shader    shader_;
    bool      wireframe_ = false;
    glm::vec3 clearColor_{0.10f, 0.25f, 0.85f};
    glm::vec3 lightDir_{0.4f, 0.7f, 0.6f};
};

} // namespace zenit
