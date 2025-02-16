#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "ImGuiCustom.hpp" // Self
#include "helper.hpp" // NOEXCEPT

#include <functional> // std::function<>{}

static void _DrawNComponents(
  char const* label, int count,
  std::function<void(ImVec2 size, int index)> draw
) NOEXCEPT {
  ImGuiWindow* window = ImGui::GetCurrentWindow();
  if (window->SkipItems) return;

  IM_ASSERT(count > 0);
  ImGui::BeginGroup();
  ImGui::PushID(label);

  ImGuiContext const* g = GImGui;
  float fullWidth = ImGui::CalcItemWidth();
  float itemInnerSpacingX = g->Style.ItemInnerSpacing.x;
  float itemWidth = fullWidth - itemInnerSpacingX * (static_cast<float>(count) - 1);
  float previousSplit = itemWidth;

  bool itemChanged = false;
  // NOTE: For-loop taken from imgui.cpp:ImGui::PushMultiItemsWidths().
  for (int index = 0; index < count; ++index) {
    float nextSplit = IM_TRUNC(itemWidth * static_cast<float>(count - index - 1) / static_cast<float>(count));
    float currentWidth = ImMax(previousSplit - nextSplit, 1.0f);
    previousSplit = nextSplit;

    ImGui::PushID(index);
    if (index > 0) ImGui::SameLine(0, itemInnerSpacingX);
    draw(ImVec2(currentWidth, 0), index);
    ImGui::PopID();
  }

  const char* labelEnd = ImGui::FindRenderedTextEnd(label);
  if (label != labelEnd) {
    ImGui::SameLine(0, itemInnerSpacingX);
    ImGui::TextEx(label, labelEnd);
  }

  ImGui::PopID();
  ImGui::EndGroup();
}

namespace ImGui {
  bool ToggleGroup(
    char const* label, int* item,
    char const* items[], int count
  ) NOEXCEPT {
    bool itemChanged = false;
    _DrawNComponents(label, count,
      [item, items, &itemChanged](ImVec2 size, int index) {
        bool active = *item == index;
        ImVec4 const* colors = ImGui::GetStyle().Colors;
        if (active) { ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]); }
        if (ImGui::Button(items[index], size) && !active) { *item = index; itemChanged = true; }
        if (active) { ImGui::PopStyleColor(); }
      }
    );
    return itemChanged;
  }

  bool ButtonFlagsGroup(
    char const* label, ImU64* flags,
    ImU64 flagsValues[], char const* flagsLabels[], int count,
    ImGuiButtonFlagsGroup options
  ) NOEXCEPT {
    bool flagsChanged = false;
    _DrawNComponents(label, count,
      [flags, flagsValues, flagsLabels, &flagsChanged, options](ImVec2 size, int index) {
        bool active = ((*flags) & flagsValues[index]) == flagsValues[index];
        ImVec4 const* colors = ImGui::GetStyle().Colors;
        if (active) { ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]); }
        if (ImGui::Button(flagsLabels[index], size)) {
          if (options & ImGuiButtonFlagsGroup_Exclusive) {
            if (active) (*flags) = 0;
            else (*flags) = flagsValues[index];
          }
          else {
            if (active) (*flags) &= ~flagsValues[index];
            else (*flags) |= flagsValues[index];
          }
          flagsChanged = true;
        }
        if (active) { ImGui::PopStyleColor(); }
      }
    );
    return flagsChanged;
  }
}
