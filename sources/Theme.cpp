#include "imgui/imgui.h" // ImVec4{}
#include "ImGuiCustom.hpp" // operator+()
#include <glad/glad.h> // glClearColor()

#include "helper.hpp" // TR_VALUE_OR(), NOEXCEPT
#include "Theme.hpp" // Self{}

TR_BEGIN_NAMESPACE()

#define TR_COLOR_L(L) TR_COLOR_RGB(L, L, L)
#define TR_COLOR_LA(L, A) TR_COLOR_RGBA(L, L, L, A)
#define TR_COLOR_RGB(R, G, B) TR_COLOR_RGBA(R, G, B, 0xFF)
#define TR_COLOR_RGBA(R, G, B, A) ImVec4( \
    static_cast<float>(R) / 255.0f, \
    static_cast<float>(G) / 255.0f, \
    static_cast<float>(B) / 255.0f, \
    static_cast<float>(A) / 255.0f  \
  )

Theme::Theme(void) NOEXCEPT {
  // Elementary
  m_colors[ColorBlack]   = TR_COLOR_L(0x00);
  m_colors[ColorWhite]   = TR_COLOR_L(0xFF);
  m_colors[ColorRed]     = TR_COLOR_RGB(0xFF, 0x00, 0x00);
  m_colors[ColorGreen]   = TR_COLOR_RGB(0x00, 0xFF, 0x00);
  m_colors[ColorBlue]    = TR_COLOR_RGB(0x00, 0x00, 0xFF);
  m_colors[ColorCyan]    = TR_COLOR_RGB(0x00, 0xFF, 0xFF);
  m_colors[ColorMagenta] = TR_COLOR_RGB(0xFF, 0x00, 0xFF);
  m_colors[ColorYellow]  = TR_COLOR_RGB(0xFF, 0xFF, 0x00);

  // Text
  m_colors[ColorText]     = TR_COLOR_L(0xFF);
  m_colors[ColorDisabled] = TR_COLOR_L(0x80);

  // Background
  m_colors[ColorWindow]       = TR_COLOR_LA(0x30, 0xF8);
  m_colors[ColorWindowBorder] = TR_COLOR_L(0x40);
  m_colors[ColorHollow]       = TR_COLOR_L(0x1F);
  m_colors[ColorHollowActive] = TR_COLOR_L(0x21);
  m_colors[ColorMenuBar]      = TR_COLOR_L(0x24);
  m_colors[ColorPopup]        = TR_COLOR_L(0x18);

  // Viewport
  m_colors[ColorGrid]         = TR_COLOR_L(0x4E);
  m_colors[ColorGridEmphasis] = TR_COLOR_L(0x55);
  m_colors[ColorViewport]     = TR_COLOR_L(0x3F);
  m_colors[ColorAxisX]        = TR_COLOR_RGB(0xF6, 0x36, 0x52);
  m_colors[ColorAxisY]        = TR_COLOR_RGB(0x6F, 0xA5, 0x1B);
  m_colors[ColorAxisZ]        = TR_COLOR_RGB(0x2F, 0x83, 0xE3);

  // Widget
  m_colors[ColorActive]           = TR_COLOR_RGB(0x47, 0x72, 0xB3);
  m_colors[ColorClickable]        = TR_COLOR_L(0x28);
  m_colors[ColorClickableActive]  = TR_COLOR_L(0x38);
  m_colors[ColorClickableHovered] = TR_COLOR_L(0x48);
  m_colors[ColorGrabbable]        = TR_COLOR_L(0x51);
  m_colors[ColorGrabbableActive]  = TR_COLOR_L(0x79);
  m_colors[ColorGrabbableHovered] = TR_COLOR_L(0x60);
}

void Theme::Apply(void) NOEXCEPT {
  ImVec4* colors = ImGui::GetStyle().Colors;
  for (size_t i = 0u; i < ImGuiCol_COUNT; ++i) {
    colors[i] = ImVec4(1.0, 0.0, 0.0, 1.0); // TODO: TMP WIP
  }

  // Text
  colors[ImGuiCol_Text]         = m_colors[ColorText];
  colors[ImGuiCol_TextDisabled] = m_colors[ColorDisabled];

  // Window
  colors[ImGuiCol_WindowBg]          = m_colors[ColorWindow];
  colors[ImGuiCol_ChildBg]           = m_colors[ColorHollow];
  colors[ImGuiCol_ResizeGrip]        = m_colors[ColorGrabbable];
  colors[ImGuiCol_ResizeGripActive]  = m_colors[ColorGrabbableActive];
  colors[ImGuiCol_ResizeGripHovered] = m_colors[ColorGrabbableHovered];

  // Border
  colors[ImGuiCol_Border]            = m_colors[ColorWindowBorder];
  colors[ImGuiCol_TableBorderLight]  = m_colors[ColorWindowBorder];
  colors[ImGuiCol_TableBorderStrong] = m_colors[ColorWindowBorder];
  colors[ImGuiCol_BorderShadow]      = TR_COLOR_LA(0x00, 0x00);

  // Dockspace
  colors[ImGuiCol_DockingPreview]        = m_colors[ColorActive];
  colors[ImGuiCol_NavWindowingHighlight] = m_colors[ColorActive];
  colors[ImGuiCol_Separator]             = m_colors[ColorWindowBorder];
  colors[ImGuiCol_SeparatorActive]       = m_colors[ColorGrabbableActive];
  colors[ImGuiCol_SeparatorHovered]      = m_colors[ColorGrabbableHovered];

  // Menu
  colors[ImGuiCol_Header]        = m_colors[ColorHollow];
  colors[ImGuiCol_HeaderHovered] = m_colors[ColorHollowActive];
  colors[ImGuiCol_HeaderActive]  = m_colors[ColorHollowActive];
  colors[ImGuiCol_MenuBarBg]     = m_colors[ColorMenuBar];
  colors[ImGuiCol_PopupBg]       = m_colors[ColorPopup];

  // Titlebar
  colors[ImGuiCol_TitleBg]          = m_colors[ColorHollow];
  colors[ImGuiCol_TitleBgActive]    = m_colors[ColorHollowActive];
  colors[ImGuiCol_TitleBgCollapsed] = m_colors[ColorHollow];

  // Widget
  colors[ImGuiCol_FrameBg]          = m_colors[ColorClickable];
  colors[ImGuiCol_FrameBgActive]    = m_colors[ColorClickableActive];
  colors[ImGuiCol_FrameBgHovered]   = m_colors[ColorClickableHovered];
  colors[ImGuiCol_Button]           = m_colors[ColorClickable];
  colors[ImGuiCol_ButtonActive]     = m_colors[ColorClickableActive];
  colors[ImGuiCol_ButtonHovered]    = m_colors[ColorClickableHovered];
  colors[ImGuiCol_SliderGrab]       = m_colors[ColorGrabbable];
  colors[ImGuiCol_SliderGrabActive] = m_colors[ColorGrabbableActive];
  colors[ImGuiCol_CheckMark]        = m_colors[ColorActive];
  colors[ImGuiCol_DragDropTarget]   = m_colors[ColorActive];

  // Tab
  colors[ImGuiCol_Tab]                       = m_colors[ColorClickable];
  colors[ImGuiCol_TabSelected]               = m_colors[ColorClickableActive];
  colors[ImGuiCol_TabHovered]                = m_colors[ColorClickableHovered];
  colors[ImGuiCol_TabSelectedOverline]       = m_colors[ColorClickableActive];
  colors[ImGuiCol_TabDimmed]                 = m_colors[ColorHollow];
  colors[ImGuiCol_TabDimmedSelected]         = m_colors[ColorClickableActive];
  colors[ImGuiCol_TabDimmedSelectedOverline] = m_colors[ColorClickableActive];

  // Scrollbar
  colors[ImGuiCol_ScrollbarBg]          = m_colors[ColorClickable];
  colors[ImGuiCol_ScrollbarGrab]        = m_colors[ColorGrabbable];
  colors[ImGuiCol_ScrollbarGrabActive]  = m_colors[ColorGrabbableActive];
  colors[ImGuiCol_ScrollbarGrabHovered] = m_colors[ColorGrabbableHovered];

  // OpenGL
  ImVec4& viewport = m_colors[ColorViewport];
  glClearColor(viewport.x, viewport.y, viewport.z, 1.0f);

  ImGuiStyle& style = ImGui::GetStyle();
  style.WindowPadding                     = ImVec2(8.00f, 8.00f);
  style.FramePadding                      = ImVec2(5.00f, 5.00f); // 5, 2
  style.CellPadding                       = ImVec2(6.00f, 6.00f);
  style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
  style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
  style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);

  style.IndentSpacing = 8;
  style.LogSliderDeadzone = 4;
  style.SeparatorTextBorderSize = 2; // 1

  style.GrabMinSize   = 14;
  style.ScrollbarSize = 18;

  static float border = 2.0f;
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

bool Theme::RenderEdit(void) NOEXCEPT {
  bool update = false;

  #define TR_SECTION(LABEL, ...) do {             \
    ImGui::SetNextItemOpen(true, ImGuiCond_Once); \
    if (ImGui::TreeNode(LABEL)) {                 \
      __VA_ARGS__; /* EVIL */                     \
      ImGui::TreePop();                           \
    }                                             \
  } while (0)

  #define TR_EDIT3(LABEL, COLOR) TR_EDIT(3, LABEL, COLOR)
  #define TR_EDIT4(LABEL, COLOR) TR_EDIT(4, LABEL, COLOR)
  #define TR_EDIT(COMPONENTS, LABEL, COLOR) do {                         \
    update |= ImGui::LargeColorEdit##COMPONENTS(LABEL, m_colors[COLOR]); \
  } while (0)

  TR_SECTION("Text",
    TR_EDIT3("Font", ColorText);
    TR_EDIT3("Disabled", ColorDisabled);
  );

  TR_SECTION("Background",
    TR_EDIT4("Window", ColorWindow);
    TR_EDIT4("Border", ColorWindowBorder);
    TR_EDIT4("Hollow", ColorHollow);
    TR_EDIT4("Active Hollow", ColorHollowActive);
    TR_EDIT3("Menu Bar", ColorMenuBar);
    TR_EDIT4("Popup", ColorPopup);
  );

  TR_SECTION("Render",
    TR_EDIT3("Grid", ColorGrid);
    TR_EDIT3("Emphasis Line ", ColorGridEmphasis);
    TR_EDIT3("Viewport", ColorViewport);
    TR_EDIT3("X Axis", ColorAxisX);
    TR_EDIT3("Y Axis", ColorAxisY);
    TR_EDIT3("Z Axis", ColorAxisZ);
  );

  TR_SECTION("Widget",
    TR_EDIT3("Active", ColorActive);
    TR_EDIT3("Clickable", ColorClickable);
    TR_EDIT3("Active Clickable", ColorClickableActive);
    TR_EDIT3("Hovered Clickable", ColorClickableHovered);
    TR_EDIT3("Grabbable", ColorGrabbable);
    TR_EDIT3("Active Grabbable", ColorGrabbableActive);
    TR_EDIT3("Hovered Grabbable", ColorGrabbableHovered);
  );

  return update;
}

TR_END_NAMESPACE()
