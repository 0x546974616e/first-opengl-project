#ifndef TR_THEME_HPP
#define TR_THEME_HPP

#include "imgui/imgui.h" // ImVec4{}
#include "helper.hpp" // NOEXCEPT
#include <cstddef> // uint8_t

#define TR_THEME_RGB(R, G, B) ImVec4( \
    static_cast<float>(R) / 255.0f,   \
    static_cast<float>(G) / 255.0f,   \
    static_cast<float>(B) / 255.0f,   \
    1.0f                              \
  )

#define TR_THEME_GRAYSCALE(H) ImVec4(              \
    static_cast<float>(TR_MIN(255, H+0)) / 255.0f, \
    static_cast<float>(TR_MIN(255, H+1)) / 255.0f, \
    static_cast<float>(TR_MIN(255, H+2)) / 255.0f, \
    1.0f \
  )

class Theme final {
public:
  void ApplyTheme(void) NOEXCEPT;
  void EditTheme(char const* name = NULL) NOEXCEPT;

private:
  struct WidgetTheme {
    ImVec4 normal, active, hovered;
    constexpr WidgetTheme(
      uint8_t normal,
      uint8_t active,
      uint8_t hovered
    ) NOEXCEPT
      : normal(TR_THEME_GRAYSCALE(normal))
      , active(TR_THEME_GRAYSCALE(active))
      , hovered(TR_THEME_GRAYSCALE(hovered))
    {}
  };

  struct FrameTheme {
    ImVec4 normal, active;
    constexpr FrameTheme(
      uint8_t normal,
      uint8_t active
    ) NOEXCEPT
      : normal(TR_THEME_GRAYSCALE(normal))
      , active(TR_THEME_GRAYSCALE(active))
    {}
  };

private:
  bool m_showStyleEditor = false;

  ImVec4 m_text = TR_THEME_RGB(0xFF, 0xFF, 0xFF);
  ImVec4 m_active = TR_THEME_RGB(0x54, 0xAB, 0xDB);
  ImVec4 m_disabled = TR_THEME_RGB(0x80, 0x80, 0x80);
  ImVec4 m_viewport = ImVec4(0.2f, 0.3f, 0.3f, 1.00f);

  FrameTheme m_window = { 0x1A, 0x24 }; // 24
  FrameTheme m_hollow = { 0x00, 0x0F };
  WidgetTheme m_grabbable = { 0x51, 0x89, 0x60 };
  WidgetTheme m_clickable = { 0x0D, 0x38, 0x30 };
  FrameTheme m_menu = { 0x24, 0x30 };
};

#endif // TR_THEME_HPP
