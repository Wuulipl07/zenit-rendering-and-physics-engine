#pragma once

namespace zenit {

// Mierzy czas klatki (delta time) i średni FPS (odświeżany co ~0.5 s).
class FrameTimer {
public:
    float tick();                 // wołaj raz na klatkę; zwraca dt w sekundach
    float fps() const { return fps_; }

private:
    double last_  = -1.0;
    double accum_ = 0.0;
    int    frames_ = 0;
    float  fps_   = 0.0f;
};

} // namespace zenit
