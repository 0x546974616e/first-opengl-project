#ifndef TR_IMGUI_CUSTOM_HPP
#define TR_IMGUI_CUSTOM_HPP

#include "imgui/imgui.h"
#include "helper.hpp" // NOEXCEPT, TR, TR_MIN(), TR_MAX()
#include <glm/vec3.hpp> // glm::vec3{}
#include <glm/vec4.hpp> // glm::vec4{}

enum ImGuiButtonFlagsGroup {
  ImGuiButtonFlagsGroup_None = 0,
  ImGuiButtonFlagsGroup_Exclusive = 1,
};

namespace ImGui {
  ///
  /// Compute multiple items widths.
  ///
  /// @return The horizontal item inner spacing.
  ///
  float GetMultiItemsWidths(int components, float itemsWidths[], float fullWidth) NOEXCEPT;
  constexpr float GetMultiItemsWidths(int components, float itemsWidths[]) NOEXCEPT {
    return GetMultiItemsWidths(components, itemsWidths, ImGui::CalcItemWidth());
  }

  ///
  /// Draw a group of toggle buttons.
  ///
  /// @returns `true` is `item` has changed, `false` otherwise.
  ///
  /// @pre Every pointers are non-NULL.
  /// @pre `length(items) == count`
  ///
  /// @post `*item` is between `0` and `count-1` or its given default value.
  ///
  bool ToggleGroup(char const* label, int* item, char const* items[], int count, float fullWidth = 0.0f) NOEXCEPT;

  ///
  /// Draw a group of button to enable/disable flags.
  ///
  /// It is the responsibility of the user to manage overlapping flags.
  ///
  /// @returns `true` is `flags` has changed, `false` otherwise.
  ///
  /// @pre `length(flagsValues) == length(flagsLabels) == count`
  /// @pre Every pointers are non-NULL.
  ///
  /// @post `*flags` is composed with one or multiple `flagsValues` (depending
  /// on `options & ImGuiButtonFlagsGroup_Exclusive`) or its given default value
  /// if nothing happens.
  ///
  bool ButtonFlagsGroup(char const* label, ImU64* flags, ImU64 flagsValues[], char const* flagsLabels[], int count, ImGuiButtonFlagsGroup options = ImGuiButtonFlagsGroup_None) NOEXCEPT;

  ///
  /// Behave like `ImGui::ColorEdit3()` but the `ImGui::ColorButton()` takes all
  /// the available space (`ImGui::CalcItemWidth()`).
  ///
  bool LargeColorEdit3(char const* label, ImVec4& color, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None, ImVec2 size = ImVec2(0, 0)) NOEXCEPT;

  ///
  /// Behave like `ImGui::ColorEdit4()` but the `ImGui::ColorButton()` takes all
  /// the available space (`ImGui::CalcItemWidth()`).
  ///
  bool LargeColorEdit4(char const* label, ImVec4& color, ImGuiColorEditFlags flags = ImGuiColorEditFlags_None, ImVec2 size = ImVec2(0, 0)) NOEXCEPT;
}

#endif // TR_IMGUI_CUSTOM_HPP
