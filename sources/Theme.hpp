#ifndef TR_THEME_HPP
#define TR_THEME_HPP

#include "imgui/imgui.h" // ImVec4{}
#include "helper.hpp" // NOEXCEPT

TR_BEGIN_NAMESPACE()

class Theme final {
public:
  enum Color {
    // Elementary
    ColorBlack = 0,
    ColorWhite,
    ColorRed,
    ColorGreen,
    ColorBlue,
    ColorCyan,
    ColorMagenta,
    ColorYellow,

    // Text
    ColorText,
    ColorDisabled,

    // Background
    ColorWindow,
    ColorWindowBorder,
    ColorHollow,
    ColorHollowActive,
    ColorMenuBar,
    ColorPopup,

    // Viewport
    ColorGrid,
    ColorGridEmphasis,
    ColorViewport,
    ColorAxisX,
    ColorAxisY,
    ColorAxisZ,

    // Widget
    ColorActive,
    ColorClickable,
    ColorClickableActive,
    ColorClickableHovered,
    ColorGrabbable,
    ColorGrabbableActive,
    ColorGrabbableHovered,

    _ColorTotal,
  };

public:
  Theme(void) NOEXCEPT;
  void Apply(void) NOEXCEPT;
  bool RenderEdit(void) NOEXCEPT;
  constexpr ImVec4 Get(Color color) const NOEXCEPT {
    TR_ASSERT(color >= 0 && color < _ColorTotal);
    return m_colors[color];
  }

private:
  bool m_showStyleEditor = false;
  ImVec4 m_colors[_ColorTotal];
};

TR_END_NAMESPACE()

#endif // TR_THEME_HPP
