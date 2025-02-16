#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"

#include "ImGuiCustom.hpp" // Self
#include "helper.hpp" // NOEXCEPT

#include <functional> // std::function<>{}

namespace TR {
  static void DrawNComponents(
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

  // NOTE: ImGui::ColorButton() could also be "hacked" to retrieve size from the (ImGui's) stack.
  static bool LargeColorEdit(char const* label, ImVec4& color, ImGuiColorEditFlags flags) {
    ImGui::PushID(label);

    bool changed = false;
    static ImVec4 backupColor;
    ImGuiContext const* g = GImGui;
    float itemInnerSpacingX = g->Style.ItemInnerSpacing.x;

    flags |= ImGuiColorEditFlags_AlphaPreviewHalf;
    ImVec2 buttonSize = ImVec2(ImGui::CalcItemWidth(), 0);
    if (ImGui::ColorButton(label, color, flags, buttonSize)) {
      ImGui::OpenPopup("##Popup");
      backupColor = color;
    }

    if (ImGui::BeginDragDropTarget()) {
      // Taken from imgui_widgets.cpp:ImGui::ColorEdit4().
      if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F)) {
        memcpy(&color.x, payload->Data, 3*sizeof(float));
        changed = true;
      }

      if (ImGuiPayload const* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F)) {
        memcpy(&color.x, payload->Data, 4*sizeof(float));
        changed = true;
      }

      ImGui::EndDragDropTarget();
    }

    const char* labelEnd = ImGui::FindRenderedTextEnd(label);
    if (label != labelEnd) {
      ImGui::SameLine(0, itemInnerSpacingX);
      ImGui::TextEx(label, labelEnd);
    }

    if (ImGui::BeginPopup("##Popup")) {
      if (label != labelEnd) {
        ImGui::TextEx(label, labelEnd);
        ImGui::Spacing();
      }

      flags |= ImGuiColorEditFlags_NoLabel;
      if (ImGui::ColorPicker4(label, &color.x, flags, &backupColor.x)) {
        changed = true;
      }

      ImGui::EndPopup();
    }

    ImGui::PopID();
    return changed;
  }
}

namespace ImGui {
  bool ToggleGroup(
    char const* label, int* item,
    char const* items[], int count
  ) NOEXCEPT {
    bool itemChanged = false;

    TR::DrawNComponents(label, count,
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

    TR::DrawNComponents(label, count,
      [flags, flagsValues, flagsLabels, &flagsChanged, options](ImVec2 size, int index) {
        bool active = ((*flags) & flagsValues[index]) == flagsValues[index];
        ImVec4 const* colors = ImGui::GetStyle().Colors;

        if (active) { ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_ButtonActive]); }
        bool clicked = ImGui::Button(flagsLabels[index], size);
        if (active) { ImGui::PopStyleColor(); }
        if (!clicked) { return; }
        flagsChanged = true;

        // When exclusivity is enabled, erase or overwrite the previous flags.
        if (options & ImGuiButtonFlagsGroup_Exclusive) {
          if (active) (*flags) = 0;
          else (*flags) = flagsValues[index];
          return;
        }

        if (active) (*flags) &= ~flagsValues[index];
        else (*flags) |= flagsValues[index];
      }
    );

    return flagsChanged;
  }

  bool LargeColorEdit3(char const* label, ImVec4& color) NOEXCEPT {
    color.w = 1.0; // Reset alpha to prevent ImGui::ColorButton() displaying it.
    return TR::LargeColorEdit(label, color, ImGuiColorEditFlags_NoAlpha);
  }

  bool LargeColorEdit4(char const* label, ImVec4& color) NOEXCEPT {
    return TR::LargeColorEdit(label, color, ImGuiColorEditFlags_AlphaBar);
  }
}
