#include "renderer/Renderer.h"

#include <string>

#ifndef ZENIT_ROOT
#define ZENIT_ROOT "."
#endif

namespace zenit {

namespace {
std::string shaderPath(const char* file) {
    return std::string(ZENIT_ROOT) + "/shaders/" + file;
}
}

Renderer::Renderer(const char* glslVersion)
    : shader_(shaderPath("basic.vert"), shaderPath("basic.frag"), glslVersion) {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
}

void Renderer::beginFrame(const Camera& camera, float aspect) {
    glClearColor(clearColor_.r, clearColor_.g, clearColor_.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_.use();
    shader_.setMat4("uView", camera.view());
    shader_.setMat4("uProj", camera.projection(aspect));
    shader_.setVec3("uLightDir", lightDir_);
}

void Renderer::draw(const Mesh& mesh, const glm::mat4& model, const glm::vec3& color, bool checker) {
    shader_.setMat4("uModel", model);
    shader_.setVec3("uColor", color);
    shader_.setInt("uChecker", checker ? 1 : 0);
    mesh.draw();
}

void Renderer::setWireframe(bool on) {
    wireframe_ = on;
    glPolygonMode(GL_FRONT_AND_BACK, on ? GL_LINE : GL_FILL);
}

} // namespace zenit
