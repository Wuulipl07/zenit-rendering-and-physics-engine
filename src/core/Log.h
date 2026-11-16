#pragma once
// Minimalne logowanie: zenit::log::info / warn / error (printf-style).
#include <cstdarg>
#include <cstdio>

namespace zenit::log {

inline void vlog(const char* level, std::FILE* out, const char* fmt, va_list args) {
    std::fprintf(out, "[%s] ", level);
    std::vfprintf(out, fmt, args);
    std::fprintf(out, "\n");
}

inline void info(const char* fmt, ...) {
    va_list a; va_start(a, fmt); vlog("info", stdout, fmt, a); va_end(a);
}
inline void warn(const char* fmt, ...) {
    va_list a; va_start(a, fmt); vlog("warn", stderr, fmt, a); va_end(a);
}
inline void error(const char* fmt, ...) {
    va_list a; va_start(a, fmt); vlog("error", stderr, fmt, a); va_end(a);
}

} // namespace zenit::log
