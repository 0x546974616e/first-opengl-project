#ifndef TR_LOG_HPP
#define TR_LOG_HPP

#include <cstdio> // FILE
#include "helper.hpp" // TR_FMTARGS()

TR_BEGIN_NAMESPACE()

/// Render logs into an ImGui window.
void GlobalLogRender(char const* title, bool* open = NULL) NOEXCEPT;
void GlobalLog(FILE* stream, char const* format, ...) NOEXCEPT TR_FMTARGS(2);

// __FILE_NAME__ is not standard.
// https://gcc.gnu.org/onlinedocs/cpp/Common-Predefined-Macros.html
// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
#define TR_FPRINTF(STREAM, FORMAT, ...)                             \
  GlobalLog(STREAM,                                                 \
    __FILE_NAME__ ":%s():" TR_STRINGIFY(__LINE__) ": " FORMAT "\n", \
    __func__, ##__VA_ARGS__                                         \
  )

#define TR_DEBUG(FORMAT, ...) TR_FPRINTF(stdout, FORMAT, ##__VA_ARGS__)
#define TR_ERROR(FORMAT, ...) TR_FPRINTF(stderr, FORMAT, ##__VA_ARGS__)

TR_END_NAMESPACE()

#endif // TR_LOG_HPP
