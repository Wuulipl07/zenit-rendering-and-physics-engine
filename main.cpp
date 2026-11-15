// Obracający się czerwony sześcian na niebieskim tle
// OpenGL 4.6 core (z fallbackiem na 3.3), GLFW + GLM, BEZ GLEW.
//
// Dlaczego bez GLEW: GLEW z repo Arch/Manjaro jest zbudowany pod GLX (X11),
// więc na sesji Wayland glewInit() zwraca błąd, mimo że okno i kontekst są OK.
// Tutaj funkcje OpenGL ładujemy sami przez glfwGetProcAddress - działa na X11 i Wayland.
//
// Zależności: sudo pacman -S base-devel glfw glm
// Kompilacja: g++ -std=c++17 main.cpp -o cube -lglfw -lGL
// Uruchomienie: ./cube

#define GLFW_INCLUDE_NONE
#include <GL/glcorearb.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstdio>
#include <cstdlib>

// ---------- Ładowanie funkcji OpenGL ----------
#define GL_FUNCS(X)                                                        \
X(PFNGLCREATESHADERPROC, glCreateShader)                               \
X(PFNGLSHADERSOURCEPROC, glShaderSource)                               \
X(PFNGLCOMPILESHADERPROC, glCompileShader)                             \
X(PFNGLGETSHADERIVPROC, glGetShaderiv)                                 \
X(PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog)                       \
X(PFNGLDELETESHADERPROC, glDeleteShader)                               \
X(PFNGLCREATEPROGRAMPROC, glCreateProgram)                             \
X(PFNGLATTACHSHADERPROC, glAttachShader)                               \
X(PFNGLLINKPROGRAMPROC, glLinkProgram)                                 \
X(PFNGLGETPROGRAMIVPROC, glGetProgramiv)                               \
X(PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog)                     \
X(PFNGLDELETEPROGRAMPROC, glDeleteProgram)                             \
X(PFNGLUSEPROGRAMPROC, glUseProgram)                                   \
X(PFNGLGETUNIFORMLOCATIONPROC, glGetUniformLocation)                   \
X(PFNGLUNIFORMMATRIX4FVPROC, glUniformMatrix4fv)                       \
X(PFNGLUNIFORM3FPROC, glUniform3f)                                     \
X(PFNGLGENVERTEXARRAYSPROC, glGenVertexArrays)                         \
X(PFNGLBINDVERTEXARRAYPROC, glBindVertexArray)                         \
X(PFNGLDELETEVERTEXARRAYSPROC, glDeleteVertexArrays)                   \
X(PFNGLGENBUFFERSPROC, glGenBuffers)                                   \
X(PFNGLBINDBUFFERPROC, glBindBuffer)                                   \
X(PFNGLBUFFERDATAPROC, glBufferData)                                   \
X(PFNGLDELETEBUFFERSPROC, glDeleteBuffers)                             \
X(PFNGLVERTEXATTRIBPOINTERPROC, glVertexAttribPointer)                 \
X(PFNGLENABLEVERTEXATTRIBARRAYPROC, glEnableVertexAttribArray)         \
X(PFNGLVIEWPORTPROC, glViewport)                                       \
X(PFNGLGETSTRINGPROC, glGetString)                                     \
X(PFNGLENABLEPROC, glEnable)                                           \
X(PFNGLCLEARCOLORPROC, glClearColor)                                   \
X(PFNGLCLEARPROC, glClear)                                             \
X(PFNGLDRAWELEMENTSPROC, glDrawElements)

#define DECLARE_FUNC(type, name) static type name = nullptr;
GL_FUNCS(DECLARE_FUNC)

static bool loadGL() {
    #define LOAD_FUNC(type, name)                                              \
    name = reinterpret_cast<type>(glfwGetProcAddress(#name));              \
    if (!name) { std::fprintf(stderr, "Brak funkcji GL: %s\n", #name); return false; }
    GL_FUNCS(LOAD_FUNC)
    return true;
}

// ---------- Shadery (bez linii #version - dodawana w locie) ----------
static const char* VERT_BODY = R"(
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vNormal;

void main() {
    vNormal = mat3(uModel) * aNormal;
    gl_Position = uProj * uView * uModel * vec4(aPos, 1.0);
}
)";

static const char* FRAG_BODY = R"(
in vec3 vNormal;
out vec4 FragColor;

uniform vec3 uColor;
uniform vec3 uLightDir;

void main() {
    vec3 n = normalize(vNormal);
    float diff = max(dot(n, normalize(uLightDir)), 0.0);
    FragColor = vec4(uColor * (0.25 + 0.75 * diff), 1.0);
}
)";

static GLuint compileShader(GLenum type, const char* version, const char* body) {
    const char* sources[2] = { version, body };
    GLuint s = glCreateShader(type);
    glShaderSource(s, 2, sources, nullptr);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetShaderInfoLog(s, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Błąd kompilacji shadera:\n%s\n", log);
        std::exit(1);
    }
    return s;
}

static GLuint createProgram(const char* version) {
    GLuint vs = compileShader(GL_VERTEX_SHADER, version, VERT_BODY);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, version, FRAG_BODY);
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glLinkProgram(p);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048];
        glGetProgramInfoLog(p, sizeof(log), nullptr, log);
        std::fprintf(stderr, "Błąd linkowania:\n%s\n", log);
        std::exit(1);
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    return p;
}

static void errorCallback(int code, const char* desc) {
    std::fprintf(stderr, "GLFW błąd %d: %s\n", code, desc);
}

static void framebufferSizeCallback(GLFWwindow*, int w, int h) {
    glViewport(0, 0, w, h);
}

static GLFWwindow* tryCreateWindow(int major, int minor) {
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    return glfwCreateWindow(800, 600, "Czerwony szescian - OpenGL", nullptr, nullptr);
}

int main() {
    glfwSetErrorCallback(errorCallback);
    if (!glfwInit()) {
        std::fprintf(stderr, "Nie udało się zainicjalizować GLFW\n");
        return 1;
    }

    // Próbujemy 4.6, jeśli się nie da - spadamy na 3.3
    const char* glslVersion = "#version 460 core\n";
    GLFWwindow* window = tryCreateWindow(4, 6);
    if (!window) {
        std::fprintf(stderr, "OpenGL 4.6 niedostępny, próbuję 3.3...\n");
        glslVersion = "#version 330 core\n";
        window = tryCreateWindow(3, 3);
    }
    if (!window) {
        std::fprintf(stderr, "Nie udało się utworzyć okna.\n");
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    if (!loadGL()) {
        glfwTerminate();
        return 1;
    }

    std::printf("GL_VERSION:  %s\n", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
    std::printf("GL_RENDERER: %s\n", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    // 24 wierzchołki (pozycja + normalna), po 4 na ścianę
    const float vertices[] = {
        // front (+Z)
        -1,-1, 1,  0, 0, 1,    1,-1, 1,  0, 0, 1,    1, 1, 1,  0, 0, 1,   -1, 1, 1,  0, 0, 1,
        // tył (-Z)
        1,-1,-1,  0, 0,-1,   -1,-1,-1,  0, 0,-1,   -1, 1,-1,  0, 0,-1,    1, 1,-1,  0, 0,-1,
        // lewa (-X)
        -1,-1,-1, -1, 0, 0,   -1,-1, 1, -1, 0, 0,   -1, 1, 1, -1, 0, 0,   -1, 1,-1, -1, 0, 0,
        // prawa (+X)
        1,-1, 1,  1, 0, 0,    1,-1,-1,  1, 0, 0,    1, 1,-1,  1, 0, 0,    1, 1, 1,  1, 0, 0,
        // góra (+Y)
        -1, 1, 1,  0, 1, 0,    1, 1, 1,  0, 1, 0,    1, 1,-1,  0, 1, 0,   -1, 1,-1,  0, 1, 0,
        // dół (-Y)
        -1,-1,-1,  0,-1, 0,    1,-1,-1,  0,-1, 0,    1,-1, 1,  0,-1, 0,   -1,-1, 1,  0,-1, 0,
    };

    unsigned int indices[36];
    for (unsigned int f = 0; f < 6; ++f) {
        unsigned int b = f * 4, i = f * 6;
        indices[i + 0] = b + 0; indices[i + 1] = b + 1; indices[i + 2] = b + 2;
        indices[i + 3] = b + 2; indices[i + 4] = b + 3; indices[i + 5] = b + 0;
    }

    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    GLuint program = createProgram(glslVersion);
    GLint locModel = glGetUniformLocation(program, "uModel");
    GLint locView  = glGetUniformLocation(program, "uView");
    GLint locProj  = glGetUniformLocation(program, "uProj");
    GLint locColor = glGetUniformLocation(program, "uColor");
    GLint locLight = glGetUniformLocation(program, "uLightDir");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    // Wymuś poprawny viewport na starcie (ważne np. na HiDPI)
    int fbw, fbh;
    glfwGetFramebufferSize(window, &fbw, &fbh);
    glViewport(0, 0, fbw, fbh);

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);

        float t = static_cast<float>(glfwGetTime());

        glClearColor(0.10f, 0.25f, 0.85f, 1.0f); // niebieskie tło
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int w, h;
        glfwGetFramebufferSize(window, &w, &h);
        float aspect = (h > 0) ? static_cast<float>(w) / h : 1.0f;

        glm::mat4 model(1.0f);
        model = glm::rotate(model, t * 0.8f, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, t * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f),
                                     glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glUseProgram(program);
        glUniformMatrix4fv(locModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(locView,  1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(locProj,  1, GL_FALSE, glm::value_ptr(proj));
        glUniform3f(locColor, 0.9f, 0.05f, 0.05f); // czerwony
        glUniform3f(locLight, 0.4f, 0.7f, 0.6f);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteProgram(program);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
