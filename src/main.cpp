// Zenit - punkt wejścia. Tu NIE ma logiki silnika: tworzymy obiekty i kręcimy pętlą.
//   Window   (core)     - okno, kontekst GL, wejście
//   Renderer (renderer) - rysowanie
//   Scene    (scene)    - obiekty + fizyka
//   Camera   (renderer) - widok; tu mapujemy na nią klawisze

#include "core/Input.h"
#include "core/Log.h"
#include "core/Timer.h"
#include "core/Window.h"
#include "renderer/Camera.h"
#include "renderer/Renderer.h"
#include "scene/Scene.h"

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>

#include <algorithm>
#include <cstdio>
#include <exception>

using namespace zenit;

namespace {

void updateCamera(Camera& camera, const Window& window, float dt) {
    if (window.cursorCaptured()) {
        const glm::vec2 d = Input::mouseDelta();
        camera.look(d.x, d.y);
    }

    auto axis = [](int plus, int minus) {
        return (Input::down(plus) ? 1.0f : 0.0f) - (Input::down(minus) ? 1.0f : 0.0f);
    };
    const float boost = Input::down(GLFW_KEY_LEFT_SHIFT) ? 3.0f : 1.0f;
    camera.move(axis(GLFW_KEY_W, GLFW_KEY_S),
                axis(GLFW_KEY_D, GLFW_KEY_A),
                axis(GLFW_KEY_E, GLFW_KEY_Q),
                dt * boost);

    if (Input::scroll() != 0.0f) {
        camera.moveSpeed = std::clamp(camera.moveSpeed * (1.0f + 0.1f * Input::scroll()), 1.0f, 60.0f);
    }
}

void printControls() {
    log::info("Sterowanie:");
    log::info("  WASD + mysz  - lot kamera (E/Q gora/dol, Shift = szybciej, kolko = predkosc)");
    log::info("  Spacja / LPM - wystrzel kule     B - 20 kul z gory     R - reset");
    log::info("  F1 - wireframe     Tab - zwolnij/zlap mysz     Esc - wyjscie");
}

} // namespace

int main() {
    try {
        Window window(1280, 720, "Zenit");
        Renderer renderer(window.glslVersion());
        Scene scene;
        Camera camera;
        FrameTimer timer;

        window.setCursorCaptured(true);
        printControls();

        double titleAccum = 0.0;

        while (!window.shouldClose()) {
            const float dt = timer.tick();

            // --- wejście ---
            Input::beginFrame();
            window.pollEvents();

            if (Input::pressed(GLFW_KEY_ESCAPE)) window.close();
            if (Input::pressed(GLFW_KEY_TAB))    window.setCursorCaptured(!window.cursorCaptured());
            if (Input::pressed(GLFW_KEY_F1))     renderer.setWireframe(!renderer.wireframe());
            if (Input::pressed(GLFW_KEY_R))      scene.reset();
            if (Input::pressed(GLFW_KEY_B))      scene.spawnBurst(20);
            if (Input::pressed(GLFW_KEY_SPACE) ||
                (window.cursorCaptured() && Input::mousePressed(GLFW_MOUSE_BUTTON_LEFT))) {
                const glm::vec3 f = camera.forward();
                scene.spawnSphere(camera.position + f * 2.0f, f * 14.0f);
            }
            updateCamera(camera, window, dt);

            // --- aktualizacja ---
            scene.update(dt);

            // --- rysowanie ---
            renderer.beginFrame(camera, window.aspect());
            scene.render(renderer);
            window.swapBuffers();

            // --- tytuł okna z FPS ---
            titleAccum += dt;
            if (titleAccum >= 0.5) {
                titleAccum = 0.0;
                char title[128];
                std::snprintf(title, sizeof(title), "Zenit | %.0f FPS | kule: %zu | wireframe: %s",
                              timer.fps(), scene.sphereCount(), renderer.wireframe() ? "tak" : "nie");
                window.setTitle(title);
            }
        }
    } catch (const std::exception& e) {
        log::error("%s", e.what());
        return 1;
    }
    return 0;
}
