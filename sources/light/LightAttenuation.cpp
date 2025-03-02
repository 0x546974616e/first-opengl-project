#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuiCustom.hpp"

#include <array> // std::array<>{}
#include <cmath> // sqrtf(), isfinite()
#include <limits> //

#include "Log.hpp"
#include "helper.hpp"
#include "light/LightAttenuation.hpp"

TR_BEGIN_NAMESPACE()

float LightAttenuation::ComputeMaxStrength(void) NOEXCEPT {
  return -1.0f * (m_constant - 1.0f / MIN_ATTENUATION);
}

float LightAttenuation::ComputeAttenuation(float distance) NOEXCEPT {
  float linear = m_strength / m_distance;
  float quadratic = (1.0f / MIN_ATTENUATION - m_constant - m_strength) / (m_distance * m_distance);
  return 1.0f / (m_constant + linear * distance + quadratic * distance * distance);
}

float LightAttenuation::FindDistance(float linear, float quadratic) NOEXCEPT {
  // c + ld + qd^2 = 1/A <=> (c-1/A) + ld + qd^2 = 0
  float constant = m_constant - (1.0f / MIN_ATTENUATION);

  if (quadratic <= 0.0f && linear <= 0.0f) {
    TR_ERROR("Linear and Quadratic terms are less or equal to zero.");
    return -1.0f;
  }

  if (quadratic <= 0.0f) {
    return -constant / linear;
  }

  float delta = linear * linear - 4.0f * quadratic * constant;

  if (delta < 0.0f) {
    TR_ERROR("Imaginary solutions (delta < 0).");
    return -1.0f;
  }

  if (delta > 0.0f) {
    return (-linear + sqrtf(delta)) / (2.0f * quadratic);
  }

  return -linear / (2.0f * quadratic);
}

void LightAttenuation::RenderSliders(bool more) NOEXCEPT {
  float maxStrength = ComputeMaxStrength();
  float strength = 1.0f - m_strength / maxStrength;
  // ImGui::DragFloat("Strength", &m_strength, maxStrength * 0.01f, 0.0f, maxStrength, "%.2f");
  ImGui::DragFloat("Strength", &strength, 0.01f, 0.0f, 1.0f, "%.2f");
  m_strength = (1.0f - strength) * maxStrength;

  if (more) {
    ImGui::SameLine();
    ImVec4* colors = ImGui::GetStyle().Colors;
    ImGui::PushStyleColor(ImGuiCol_Text, colors[ImGuiCol_TextDisabled]);
    ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_WindowBg]);
    if (ImGui::Button("(+)##Attenuation")) ImGui::OpenPopup("##More Attenuation");
    ImGui::PopStyleColor(2);
  }

  ImGui::DragFloat("Distance", &m_distance, 0.1f, 0.1f, 50.0f, "%.1f");
}

void LightAttenuation::RenderUi(void) NOEXCEPT {
  RenderSliders(true);

  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(ImMin(viewport->Size * 0.8f, ImVec2(300, 0)));
  ImGui::SetNextWindowViewport(viewport->ID);

  if (ImGui::BeginPopup("##More Attenuation")) {
    ImGui::Text("Light Attenuation");
    ImGui::Separator();

    int item = static_cast<int>(m_useTerms);
    static char const* buttons[2] = { "Strength", "Terms" };
    float regionWidth = ImGui::GetContentRegionAvail().x;
    ImGui::ToggleGroup("##Mode", &item, buttons, 2, regionWidth);
    m_useTerms = item == 1;

    static std::array<float, 32> points = { 0.0f };
    static std::array<char, 16> buffer = { 0 };

    float maxDistance = m_distance; // * 1.6666f;
    for (size_t i = 0u; i < points.size(); ++i) {
      points[i] = ComputeAttenuation(
        static_cast<float>(i) / points.size() * maxDistance
      );
    }

    /*
     * O-------------------A---o
     * |',                 | 1 |
     * |  '-,              |   |
     * |     '--,_         |   |
     * |          '---,.__ | 0 |
     * B-------------------o---o
     * | 0        Distance |
     * o-------------------o
     */

    ImVec2 zeroSize = ImGui::CalcTextSize("0");
    ImVec2 spacing = ImGui::GetStyle().ItemInnerSpacing;
    ImVec2 plotSize = ImVec2(regionWidth - spacing.x - zeroSize.x, 80.0f);
    ImVec2 O = ImGui::GetCursorScreenPos();
    ImGui::PlotLines("##Attenuation", points.data(), points.size(), 0u, NULL, 0.0f, 1.0f / m_constant, plotSize);
    ImGui::SameLine();
    ImVec2 A = ImGui::GetCursorPos();
    ImGui::Text("1");
    ImVec2 B = ImGui::GetCursorPos();
    ImGui::SetCursorPos(A + ImVec2(0.0f, plotSize.y - zeroSize.y - spacing.y));
    ImGui::Text("0"); // Right "0"
    ImGui::SetCursorPos(B + ImVec2(spacing.x, -1.0f * spacing.y / 2.0f));
    ImGui::Text("0"); // Below "0"
    ImGui::SameLine();
    ImFormatString(buffer.data(), buffer.size(), "%.1f", maxDistance);
    ImVec2 textSize = ImGui::CalcTextSize(buffer.data());
    ImGui::SetCursorPosX(A.x - textSize.x - 2.0f * spacing.x);
    ImGui::TextUnformatted(buffer.data());

    if (false) { // TMP
      ImGui::SameLine();
      float ratio = m_distance / maxDistance;
      ImFormatString(buffer.data(), buffer.size(), "%.1f", m_distance);
      ImVec2 textSize = ImGui::CalcTextSize(buffer.data());
      ImGui::SetCursorPosX(plotSize.x * ratio - textSize.x / 2.0f + spacing.x);
      ImGui::TextUnformatted(buffer.data());

      ImGui::GetWindowDrawList()->AddLine(
        O + ImVec2(plotSize.x * ratio, 0.0f),
        O + ImVec2(plotSize.x * ratio, plotSize.y),
        ImGui::GetColorU32(ImGui::GetStyle().Colors[ImGuiCol_PlotLinesHovered]),
        1.0f
      );
    }

    if (true) { // TMP
      constexpr size_t N = 4u;
      ImU32 color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 0.1f));
      for (size_t i = 1u; i < N; ++i) {
        ImGui::GetWindowDrawList()->AddLine(
          O + ImVec2(1 + plotSize.x * static_cast<float>(i) / N, 0.0f),
          O + ImVec2(1 + plotSize.x * static_cast<float>(i) / N, plotSize.y),
          color, 1.0f
        );
      }
    }

    ImGui::PushItemWidth(regionWidth * 0.725f);

    if (m_useTerms) {
      ImGui::BeginDisabled();
      // NOTE: Update PlotLines is enabled.
      ImGui::DragFloat("Constant", &m_constant, 0.01f, 0.01f, 2.0f, "%.2f");
      ImGui::EndDisabled();

      constexpr float step = 0.01f;
      float quadratic = (1.0f / MIN_ATTENUATION - m_constant - m_strength) / (m_distance * m_distance);
      float linear = m_strength / m_distance;

      bool l = ImGui::DragFloat("Linear", &linear, step, 0.00f, 2.00f, "%.2f");
      bool q = ImGui::DragFloat("Quadratic", &quadratic, step, 0.00f, 2.00f, "%.2f");

      if (linear <= 0.0f && quadratic <= 0.0f) {
        if (l) linear = step; else if (q) quadratic = step;
      }

      float distance = FindDistance(linear, quadratic);
      if (distance <= 0.0f) {
        TR_ERROR("Unable to retrieve the distance.");
      }
      else {
        m_strength = linear * distance;
        m_distance = distance;
      }
    }
    else {
      RenderSliders(false);
    }

    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }
}

TR_END_NAMESPACE()
