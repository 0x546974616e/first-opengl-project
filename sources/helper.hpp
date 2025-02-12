#ifndef TR_HELPER_HPP
#define TR_HELPER_HPP

#define IN
#define OUT
#define INOUT

// TODO: -fno-exceptions
#define NOEXCEPT noexcept

#define MALLOCPTR(TYPE) TYPE*
#define STATICPTR(TYPE) TYPE const*

#define UNUSED __attribute__((unused)) // [[maybe_unused]]

#define TR_STRINGIFY_HELPER(X) #X
#define TR_STRINGIFY(X) TR_STRINGIFY_HELPER(X)

#define TR_DELETE_CTOR(CLASS, REFERENCE) \
  CLASS& operator=(CLASS REFERENCE) = delete; \
  CLASS(CLASS REFERENCE) = delete;

#define TR_DELETE_COPY_CTOR(CLASS) TR_DELETE_CTOR(CLASS, const&)
#define TR_DELETE_MOVE_CTOR(CLASS) TR_DELETE_CTOR(CLASS, &&)

#if defined(__MINGW32__) && !defined(__clang__)
#define TR_FMTARGS(POSITION) __attribute__((format(gnu_printf, POSITION, POSITION+1)))
#define TR_FMTLIST(POSITION) __attribute__((format(gnu_printf, POSITION, 0)))
#elif defined(__clang__) || defined(__GNUC__)
#define TR_FMTARGS(POSITION) __attribute__((format(printf, POSITION, POSITION+1)))
#define TR_FMTLIST(POSITION) __attribute__((format(printf, POSITION, 0)))
#else
#define TR_FMTARGS(POSITION)
#define TR_FMTLIST(POSITION)
#endif

#define TR_ASSERT(EXPRESSION) assert(EXPRESSION)
#define TR_ASSERT_RECOVERABLE(EXPRESSION)          \
  do { if (!(EXPRESSION)) TR_ERROR(                \
    "assert(" TR_STRINGIFY(EXPRESSION) ") failed." \
  ); } while (0)

/// Size of a static C-style array. Don't use on pointers!
#define TR_ARRAYSIZE(ARRAY) ((size_t) (sizeof(ARRAY) / sizeof(*(ARRAY))))

#define TR_VALUE_OR(VALUE, OR) ((VALUE) ? (VALUE) : (OR))

#define TR_MIN(A, B) (((A) < (B)) ? (A) : (B))
#define TR_MAX(A, B) (((A) > (B)) ? (A) : (B))
#define TR_CLAMP(X, A, B) TR_MAX(A, TR_MIN(B, X))

#ifdef IMGUI_VERSION
  static constexpr ImVec2 operator*(ImVec2 const& vector, float scalar) NOEXCEPT {
    return ImVec2(vector.x * scalar, vector.y * scalar);
  }

  static constexpr ImVec2 operator+(ImVec2 const& a, ImVec2 const& b) NOEXCEPT {
    return ImVec2(a.x + b.x, a.y + b.y);
  }

  static constexpr ImVec2 operator-(ImVec2 const& a, ImVec2 const& b) NOEXCEPT {
    return ImVec2(a.x - b.x, a.y - b.y);
  }

  static constexpr ImVec2 Min(ImVec2 const& a, ImVec2 const& b) NOEXCEPT {
    return ImVec2(TR_MIN(a.x, b.x), TR_MIN(a.y, b.y));
  }

  static constexpr ImVec2 Max(ImVec2 const& a, ImVec2 const& b) NOEXCEPT {
    return ImVec2(TR_MAX(a.x, b.x), TR_MAX(a.y, b.y));
  }
#endif

#endif // TR_HELPER_HPP
