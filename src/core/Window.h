#pragma once
#include <string>

struct GLFWwindow;

namespace zenit {

// Okno + kontekst OpenGL + załadowane funkcje GL + podpięte wejście.
// Próbuje OpenGL 4.6 core, w razie braku spada na 3.3 (glslVersion() zwraca
// pasującą linię "#version ...").
class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    bool shouldClose() const;
    void close();
    void pollEvents();
    void swapBuffers();

    void setTitle(const std::string& title);
    void setCursorCaptured(bool captured);    // true = mysz "przyklejona" (FPS-look)
    bool cursorCaptured() const { return captured_; }

    int   framebufferWidth() const;
    int   framebufferHeight() const;
    float aspect() const;

    const char* glslVersion() const { return glslVersion_; }
    GLFWwindow* handle() const { return window_; }

private:
    GLFWwindow* window_ = nullptr;
    const char* glslVersion_ = "#version 460 core\n";
    bool captured_ = false;
};

} // namespace zenit
