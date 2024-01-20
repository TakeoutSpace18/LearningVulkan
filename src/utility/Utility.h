#ifndef UTILITY_H
#define UTILITY_H

#include <spdlog/spdlog.h>

#ifndef NDEBUG
#define ASSERTIONS_ENABLED
#endif

#ifdef ASSERTIONS_ENABLED

#define debugBreak() __builtin_trap()

inline void reportAssertionFailure(const char* file, int line, const char* msg)
{
    spdlog::error("{}:{} Assertion failed: {}", file, line, msg);
}

#define ASSERT(expr)                                            \
    if (expr) {}                                                \
    else                                                        \
    {                                                           \
        reportAssertionFailure(__FILE__, __LINE__, #expr);      \
        debugBreak();                                           \
    }

#else
#define ASSERT(expr)
#endif

#define NODISCARD [[nodiscard]]

#endif //UTILITY_H
