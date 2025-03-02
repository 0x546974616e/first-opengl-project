#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuiCustom.hpp"

#include "light/LightColor.hpp"

TR_BEGIN_NAMESPACE()

void LightColor::UpdateFromSpecular(void) NOEXCEPT {
  float h, s, v;
  ImGui::ColorConvertRGBtoHSV(m_specular.x, m_specular.y, m_specular.z, h, s, v);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_diffuseFactor, m_diffuse.x, m_diffuse.y, m_diffuse.z);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_ambientFactor, m_ambient.x, m_ambient.y, m_ambient.z);
}

void LightColor::UpdateFromDiffuse(void) NOEXCEPT {
  float h, s, v;
  ImGui::ColorConvertRGBtoHSV(m_diffuse.x, m_diffuse.y, m_diffuse.z, h, s, v);
  ImGui::ColorConvertHSVtoRGB(h, s, v /= m_diffuseFactor, m_specular.x, m_specular.y, m_specular.z);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_ambientFactor, m_ambient.x, m_ambient.y, m_ambient.z);
}

void LightColor::UpdateFromAmbient(void) NOEXCEPT {
  float h, s, v;
  ImGui::ColorConvertRGBtoHSV(m_ambient.x, m_ambient.y, m_ambient.z, h, s, v);
  ImGui::ColorConvertHSVtoRGB(h, s, v /= m_ambientFactor, m_specular.x, m_specular.y, m_specular.z);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_diffuseFactor, m_diffuse.x, m_diffuse.y, m_diffuse.z);
}

void LightColor::UpdateDiffuse(void) NOEXCEPT {
  float h, s, v;
  ImGui::ColorConvertRGBtoHSV(m_specular.x, m_specular.y, m_specular.z, h, s, v);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_diffuseFactor, m_diffuse.x, m_diffuse.y, m_diffuse.z);
}

void LightColor::UpdateAmbient(void) NOEXCEPT {
  float h, s, v;
  ImGui::ColorConvertRGBtoHSV(m_specular.x, m_specular.y, m_specular.z, h, s, v);
  ImGui::ColorConvertHSVtoRGB(h, s, v * m_ambientFactor, m_ambient.x, m_ambient.y, m_ambient.z);
}

void LightColor::RenderUi(void) NOEXCEPT {
  if (m_bindColors) {
    if (ImGui::LargeColorEdit3("Color", m_specular)) {
      UpdateFromSpecular();
    }
  }
  else {
    float itemsWidths[3] = { 0.0f, 0.0f, 0.0f };
    ImGui::GetMultiItemsWidths(3, itemsWidths, ImGui::CalcItemWidth());
    ImGui::LargeColorEdit3("##Specular", m_specular, 0, ImVec2(itemsWidths[0], 0));
    ImGui::SameLine();
    ImGui::LargeColorEdit3("##Diffuse", m_diffuse, 0, ImVec2(itemsWidths[1], 0));
    ImGui::SameLine();
    ImGui::LargeColorEdit3("##Ambient", m_ambient, 0, ImVec2(itemsWidths[2], 0));
    ImGui::SameLine();
    ImGui::Text("Color*");
  }

  ImGui::SameLine();
  ImVec4* colors = ImGui::GetStyle().Colors;
  ImGui::PushStyleColor(ImGuiCol_Text, colors[ImGuiCol_TextDisabled]);
  ImGui::PushStyleColor(ImGuiCol_Button, colors[ImGuiCol_WindowBg]);
  if (ImGui::Button("(+)##Colors")) ImGui::OpenPopup("##More Colors");
  ImGui::PopStyleColor(2);

  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowSize(ImMin(viewport->Size * 0.8f, ImVec2(300, 0)));
  ImGui::SetNextWindowViewport(viewport->ID);

  if (ImGui::BeginPopup("##More Colors")) {
    ImGui::Text("Light Color");
    ImGui::Separator();

    bool changed = false;
    // TODO: Cumbersome... Really needed?
    float itemInnerSpacingX = ImGui::GetStyle().ItemInnerSpacing.x;
    float regionWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("Specular").x - itemInnerSpacingX * 2.0f;
    ImGui::PushItemWidth(regionWidth / 2.0f);

    changed = ImGui::LargeColorEdit3("##Specular", m_specular);
    if (changed && m_bindColors) UpdateFromSpecular();
    ImGui::SameLine(0.0f, regionWidth / 2.0f + itemInnerSpacingX * 2.0f);
    ImGui::Text("Specular");

    changed = ImGui::LargeColorEdit3("##Diffuse1", m_diffuse); ImGui::SameLine();
    if (changed && m_bindColors) UpdateFromDiffuse();
    if (!m_bindColors) ImGui::BeginDisabled();
    changed = ImGui::DragFloat("##Diffuse2", &m_diffuseFactor, 0.001f, 0.0f, 1.0f, "%.3f%%");
    if (changed && m_bindColors) UpdateDiffuse();
    if (!m_bindColors) ImGui::EndDisabled();
    ImGui::SameLine(); ImGui::Text("Diffuse");

    changed = ImGui::LargeColorEdit3("##Ambient1", m_ambient); ImGui::SameLine();
    if (changed && m_bindColors) UpdateFromAmbient();
    if (!m_bindColors) ImGui::BeginDisabled();
    changed = ImGui::DragFloat("##Ambient2", &m_ambientFactor, 0.001f, 0.0f, 1.0f, "%.3f%%");
    if (changed && m_bindColors) UpdateAmbient();
    if (!m_bindColors) ImGui::EndDisabled();
    ImGui::SameLine(); ImGui::Text("Ambient");

    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + regionWidth / 2.0f + itemInnerSpacingX);
    if (ImGui::Checkbox("Bind colors", &m_bindColors) && m_bindColors) {
      UpdateFromSpecular();
    }

    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }
}

TR_END_NAMESPACE()
