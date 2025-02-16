#ifndef TR_IMGUI_CUSTOM_HPP
#define TR_IMGUI_CUSTOM_HPP

#include "imgui/imgui.h"
#include "helper.hpp" // NOEXCEPT, TR_MIN(), TR_MAX()

enum ImGuiButtonFlagsGroup {
  ImGuiButtonFlagsGroup_None = 0,
  ImGuiButtonFlagsGroup_Exclusive = 1,
};

namespace ImGui {
  ///
  /// Draw a group of toggle buttons.
  ///
  /// @returns `true` is `item` has changed, `false` otherwise.
  ///
  bool ToggleGroup(char const* label, int* item, char const* items[], int count) NOEXCEPT;

  ///
  /// Draw a group of button to enable/disable flags.
  ///
  /// It is the responsibility of the user to manage overlapping flags.
  ///
  /// @returns `true` is `flags` has changed, `false` otherwise.
  ///
  bool ButtonFlagsGroup(char const* label, ImU64* flags, ImU64 flagsValues[], char const* flagsLabels[], int count, ImGuiButtonFlagsGroup options = ImGuiButtonFlagsGroup_None) NOEXCEPT;
}

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

#endif // TR_IMGUI_CUSTOM_HPP
