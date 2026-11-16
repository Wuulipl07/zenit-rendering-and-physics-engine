#pragma once
#include <glm/glm.hpp>

struct GLFWwindow;

namespace zenit {

// Statyczna klasa wejścia. Użycie w każdej klatce:
//   Input::beginFrame();   // czyści "pressed" i delta myszy
//   window.pollEvents();   // GLFW wypełnia stan
//   if (Input::pressed(GLFW_KEY_SPACE)) ...
class Input {
public:
    static void attach(GLFWwindow* window);   // woła Window
    static void beginFrame();
    static void resetMouse();                 // po zmianie trybu kursora

    static bool down(int key);                // klawisz trzymany
    static bool pressed(int key);             // wciśnięty w tej klatce
    static bool mouseDown(int button);
    static bool mousePressed(int button);

    static glm::vec2 mouseDelta();            // ruch myszy w tej klatce (piksele)
    static float scroll();                    // przewinięcie kółka w tej klatce
};

} // namespace zenit
