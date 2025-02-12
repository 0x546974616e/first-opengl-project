#include "imgui/imgui.h" // ImVec4{}
#include <glad/glad.h> // glClearColor()

#include "helper.hpp" // TR_VALUE_OR(), NOEXCEPT
#include "Theme.hpp" // Self{}

void Theme::ApplyTheme(void) NOEXCEPT {
  ImVec4* colors = ImGui::GetStyle().Colors;
  for (size_t i = 0u; i < ImGuiCol_COUNT; ++i) {
    colors[i] = ImVec4(1.0, 0.0, 0.0, 1.0); // TODO: TMP
  }

  // Text
  colors[ImGuiCol_Text] = m_text;
  colors[ImGuiCol_TextDisabled] = m_disabled;

  // Window
  colors[ImGuiCol_WindowBg] = m_window.normal;
  colors[ImGuiCol_WindowBg].w = 0.95f;
  colors[ImGuiCol_ChildBg] = m_hollow.normal;
  colors[ImGuiCol_ResizeGrip] = m_grabbable.normal;
  colors[ImGuiCol_ResizeGripHovered] = m_grabbable.hovered;
  colors[ImGuiCol_ResizeGripActive] = m_grabbable.active;

  // Border
  colors[ImGuiCol_Border] = m_window.active;
  colors[ImGuiCol_TableBorderLight] = m_window.active;
  colors[ImGuiCol_TableBorderStrong] = m_window.active;
  colors[ImGuiCol_BorderShadow].w = 0.0f;

  // Dockspace
  colors[ImGuiCol_DockingPreview] = m_active;
  colors[ImGuiCol_NavWindowingHighlight] = m_active;
  colors[ImGuiCol_Separator] = m_window.active;
  colors[ImGuiCol_SeparatorHovered] = m_grabbable.hovered;
  colors[ImGuiCol_SeparatorActive] = m_grabbable.active;

  // Menu
  colors[ImGuiCol_Header] = m_hollow.normal;
  colors[ImGuiCol_HeaderHovered] = m_hollow.active;
  colors[ImGuiCol_HeaderActive] = m_hollow.active;
  colors[ImGuiCol_MenuBarBg] = m_menu.normal;
  colors[ImGuiCol_PopupBg] = m_menu.active;

  // Titlebar
  colors[ImGuiCol_TitleBg] = m_hollow.normal;
  colors[ImGuiCol_TitleBgActive] = m_hollow.active;
  colors[ImGuiCol_TitleBgCollapsed] = m_hollow.normal;

  // Widget
  colors[ImGuiCol_FrameBg] = m_clickable.normal;
  colors[ImGuiCol_FrameBgHovered] = m_clickable.hovered;
  colors[ImGuiCol_FrameBgActive] = m_clickable.active;
  colors[ImGuiCol_Button] = m_clickable.normal;
  colors[ImGuiCol_ButtonHovered] = m_clickable.hovered;
  colors[ImGuiCol_ButtonActive] = m_clickable.active;
  colors[ImGuiCol_SliderGrab] = m_grabbable.normal;
  colors[ImGuiCol_SliderGrabActive] = m_grabbable.active;
  colors[ImGuiCol_CheckMark] = m_active;
  colors[ImGuiCol_DragDropTarget] = m_active;

  // Tab
  colors[ImGuiCol_Tab] = m_clickable.normal;
  colors[ImGuiCol_TabHovered] = m_clickable.hovered;
  colors[ImGuiCol_TabSelected] = m_clickable.active;
  colors[ImGuiCol_TabSelectedOverline] = m_active;
  colors[ImGuiCol_TabDimmed] = m_hollow.normal;
  colors[ImGuiCol_TabDimmedSelected] = m_clickable.active;
  colors[ImGuiCol_TabDimmedSelectedOverline] = m_active;

  // Scrollbar
  colors[ImGuiCol_ScrollbarBg] = m_clickable.normal;
  colors[ImGuiCol_ScrollbarGrab] = m_grabbable.normal;
  colors[ImGuiCol_ScrollbarGrabHovered] = m_grabbable.hovered;
  colors[ImGuiCol_ScrollbarGrabActive] = m_grabbable.active;

  // OpenGL
  glClearColor(m_viewport.x, m_viewport.y, m_viewport.z, 1.0f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding                     = ImVec2(8.00f, 8.00f);
  style.FramePadding                      = ImVec2(5.00f, 5.00f); // 5, 2
  style.CellPadding                       = ImVec2(6.00f, 6.00f);
  style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);

  style.IndentSpacing = 30;
  style.LogSliderDeadzone = 4;
  style.SeparatorTextBorderSize = 2; // 1

  style.GrabMinSize   = 14;
  style.ScrollbarSize = 18;

  static float border = 1.0f;
  style.WindowBorderSize = border;
  style.ChildBorderSize = border;
  style.PopupBorderSize = border;
  style.FrameBorderSize = 0.0f;
  style.TabBorderSize = 0.0f; // border
  style.TabBarBorderSize = 0.0f; // border;
  style.TabBarOverlineSize = 0.0f;
  style.DockingSeparatorSize = border;

  static float rounding = 4.0f;
  style.WindowRounding = rounding;
  style.ChildRounding = rounding;
  style.FrameRounding = rounding;
  style.PopupRounding = rounding;
  style.ScrollbarRounding = rounding;
  style.GrabRounding = rounding;
  style.TabRounding = rounding;

  style.WindowMenuButtonPosition = ImGuiDir_None;
  style.ColorButtonPosition = ImGuiDir_Left;
}

static bool BigColorEdit(char const* id, ImVec4& color, ImVec2 dimensions) NOEXCEPT {
  ImGui::PushID(id);

  bool changed = false;
  static ImVec4 backupColor;

  if (ImGui::ColorButton(id, color, ImGuiColorEditFlags_None, dimensions)) {
    ImGui::OpenPopup("##Popup");
    backupColor = color;
  }

  if (ImGui::BeginDragDropTarget()) {
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

  if (ImGui::BeginPopup("##Popup")) {
    ImGuiColorEditFlags picker = ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview;
    ImGuiColorEditFlags button = ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf;

    // Unfortunately __attribute__(format(printf, 1, 0)) does not allow the id
    // to be passed directly to ImGui::Text().
    ImGui::Text("%s", id);

    if (ImGui::ColorPicker3("##Picker", &color.x, picker)) {
      changed = true;
    }

    ImGui::SameLine();
    ImGui::BeginGroup();

    ImGui::Text("Current");
    ImVec2 size = ImVec2(60, 40);
    ImGui::ColorButton("##Current", color, button, size);

    ImGui::Text("Previous");
    if (ImGui::ColorButton("##Previous", backupColor, button, size)) {
      color = backupColor;
      changed = true;
    }

    ImGui::EndGroup();
    ImGui::EndPopup();
  }

  ImGui::PopID();
  return changed;
}

void Theme::EditTheme(char const* name) NOEXCEPT {
  if (ImGui::Begin(TR_VALUE_OR(name, "Theme"))) {
    #define TR_CELL_WIDTH  50
    #define TR_CELL_HEIGHT 30
    #define TR_CELL_DIMENSIONS ImVec2(TR_CELL_WIDTH, TR_CELL_HEIGHT)

    bool update = false;
    #define TR_COLOR_EDIT(ID, COLOR) do {                \
      if (BigColorEdit(ID, COLOR, TR_CELL_DIMENSIONS)) { \
        update = true;                                   \
      }                                                  \
    } while (0)

    #define TR_HALIGN(TEXT, WIDTH) do {                           \
      float x = ImGui::GetCursorPosX();                           \
      float offset = ((WIDTH) - ImGui::CalcTextSize(TEXT).x) / 2; \
      ImGui::SetCursorPosX(x + offset); ImGui::Text(TEXT);        \
      ImGui::SetCursorPosX(x);                                    \
    } while (0)

    #define TR_VALIGN(TEXT, HEIGHT) do {                           \
      float y = ImGui::GetCursorPosY();                            \
      float offset = ((HEIGHT) - ImGui::CalcTextSize(TEXT).y) / 2; \
      ImGui::SetCursorPosY(y + offset); ImGui::Text(TEXT);         \
      ImGui::SetCursorPosY(y);                                     \
    } while (0)

    #define TR_WIDGET_EDIT(NAME, COLOR) do {                                       \
      ImGui::TableNextRow();                                                       \
      ImGui::TableNextColumn(); TR_VALIGN(NAME, TR_CELL_HEIGHT);                   \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Normal" , (COLOR).normal);  \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Active" , (COLOR).active);  \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Hovered", (COLOR).hovered); \
    } while (0)

    #define TR_FRAME_EDIT(NAME, COLOR) do {                                      \
      ImGui::TableNextRow();                                                     \
      ImGui::TableNextColumn(); TR_VALIGN(NAME, TR_CELL_HEIGHT);                 \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Normal", (COLOR).normal); \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Active", (COLOR).active); \
      ImGui::TableNextColumn();                                                  \
    } while (0)

    #define TR_SINGLE_EDIT(NAME, COLOR) do {                              \
      ImGui::TableNextRow();                                              \
      ImGui::TableNextColumn(); TR_VALIGN(NAME, TR_CELL_HEIGHT);          \
      ImGui::TableNextColumn(); TR_COLOR_EDIT(NAME " - Normal", (COLOR)); \
      ImGui::TableNextColumn();                                           \
      ImGui::TableNextColumn();                                           \
    } while (0)

    // | ImGuiTableFlags_BordersInner;
    float cellX = ImGui::GetStyle().CellPadding.x;
    ImGuiTabBarFlags flags = ImGuiTableFlags_SizingFixedSame;
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(cellX, 2.0f));
    if (ImGui::BeginTable("Theme", 4, flags)) {
      ImGui::TableNextRow();
      ImGui::TableNextColumn();
      ImGui::TableNextColumn(); TR_HALIGN("Normal" , TR_CELL_WIDTH);
      ImGui::TableNextColumn(); TR_HALIGN("Active" , TR_CELL_WIDTH);
      ImGui::TableNextColumn(); TR_HALIGN("Hovered", TR_CELL_WIDTH);

      TR_SINGLE_EDIT("Text", m_text);
      TR_SINGLE_EDIT("Disabled", m_disabled);
      TR_SINGLE_EDIT("Active", m_active);
      TR_SINGLE_EDIT("Viewport", m_viewport);
      TR_FRAME_EDIT("Window", m_window);
      TR_FRAME_EDIT("Hollow", m_hollow);
      TR_WIDGET_EDIT("Clickable", m_clickable);
      TR_WIDGET_EDIT("Grabbable", m_grabbable);
      TR_FRAME_EDIT("Menu", m_menu);
      ImGui::EndTable();
    }
    ImGui::PopStyleVar();

    ImGui::Checkbox("Show Style Editor", &m_showStyleEditor);

    if (update) {
      ApplyTheme();
    }
  }
  ImGui::End();

  if (m_showStyleEditor) {
    // Viewport can never be NULL.
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowSize(Min(viewport->Size * 0.8f, ImVec2(500, 800)), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(viewport->GetCenter(), ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));
    ImGui::Begin("Style Editor", &m_showStyleEditor);
    ImGui::ShowStyleEditor();
    ImGui::End();
  }
}
