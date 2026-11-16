#include "renderer/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace zenit {

namespace {

std::string readFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Nie mozna otworzyc pliku shadera: " + path);
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint compile(GLenum type, const char* version, const std::string& body, const std::string& name) {
    const char* sources[2] = { version, body.c_str() };
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 2, sources, nullptr);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char infoLog[2048];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
        glDeleteShader(shader);
        throw std::runtime_error("Blad kompilacji shadera " + name + ":\n" + infoLog);
    }
    return shader;
}

} // namespace

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath, const char* glslVersion) {
    GLuint vs = compile(GL_VERTEX_SHADER,   glslVersion, readFile(vertexPath),   vertexPath);
    GLuint fs = compile(GL_FRAGMENT_SHADER, glslVersion, readFile(fragmentPath), fragmentPath);

    program_ = glCreateProgram();
    glAttachShader(program_, vs);
    glAttachShader(program_, fs);
    glLinkProgram(program_);
    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint ok = 0;
    glGetProgramiv(program_, GL_LINK_STATUS, &ok);
    if (!ok) {
        char infoLog[2048];
        glGetProgramInfoLog(program_, sizeof(infoLog), nullptr, infoLog);
        glDeleteProgram(program_);
        program_ = 0;
        throw std::runtime_error(std::string("Blad linkowania shadera:\n") + infoLog);
    }
}

Shader::~Shader() {
    if (program_) glDeleteProgram(program_);
}

Shader::Shader(Shader&& other) noexcept
    : program_(other.program_), locations_(std::move(other.locations_)) {
    other.program_ = 0;
}

Shader& Shader::operator=(Shader&& other) noexcept {
    if (this != &other) {
        if (program_) glDeleteProgram(program_);
        program_ = other.program_;
        locations_ = std::move(other.locations_);
        other.program_ = 0;
    }
    return *this;
}

void Shader::use() const { glUseProgram(program_); }

GLint Shader::location(const std::string& name) const {
    auto it = locations_.find(name);
    if (it != locations_.end()) return it->second;
    GLint loc = glGetUniformLocation(program_, name.c_str());
    locations_[name] = loc;
    return loc;
}

void Shader::setInt(const std::string& n, int v) const   { glUniform1i(location(n), v); }
void Shader::setFloat(const std::string& n, float v) const { glUniform1f(location(n), v); }
void Shader::setVec3(const std::string& n, const glm::vec3& v) const { glUniform3f(location(n), v.x, v.y, v.z); }
void Shader::setMat4(const std::string& n, const glm::mat4& m) const {
    glUniformMatrix4fv(location(n), 1, GL_FALSE, glm::value_ptr(m));
}

} // namespace zenit
