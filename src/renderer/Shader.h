#pragma once
#include "renderer/GLLoader.h"

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace zenit {

// Program shaderowy ładowany z plików (.vert / .frag).
// Linia "#version" jest doklejana automatycznie (zależnie od wersji GL okna).
// Uwaga: set*() wymagają wcześniejszego use().
class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath, const char* glslVersion);
    ~Shader();
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;
    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void use() const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLint location(const std::string& name) const;

    GLuint program_ = 0;
    mutable std::unordered_map<std::string, GLint> locations_;
};

} // namespace zenit
