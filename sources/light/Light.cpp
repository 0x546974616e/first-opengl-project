#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuiCustom.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "helper.hpp" // NOEXCEPT
#include "light/Light.hpp" // Self{}

TR_BEGIN_NAMESPACE()

static bool RenderDirection(glm::vec3& direction) NOEXCEPT {
  float length = glm::length(direction);

  float angles[2] = {
    direction.x == 0.0f ? 0.0f : atan2f(direction.z, direction.x), // XZ
    glm::acos(direction.y / length) // Y
  };

  angles[0] = glm::degrees(angles[0]);
  angles[1] = glm::degrees(angles[1]);

  bool changed = false;
  ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
  changed |= ImGui::DragFloat("##Phi", angles + 0, 1.0f, -360.0f, 360.0f, "%.0f"); ImGui::SameLine();
  changed |= ImGui::DragFloat("##Theta", angles + 1, 1.0f, 0.1f, 179.9f, "%.0f"); ImGui::SameLine();
  ImGui::PopItemWidth(); ImGui::PopItemWidth();
  ImGui::Text("Direction");

  angles[0] = glm::radians(+angles[0]);
  angles[1] = glm::radians(-angles[1]);

  float x = -length * glm::sin(angles[1]);
  float y = +length * glm::cos(angles[1]);

  direction.x = x * glm::cos(angles[0]);
  direction.y = y;
  direction.z = x * glm::sin(angles[0]);

  direction = glm::normalize(direction);

  return changed;
}

DirectionalLight::DirectionalLight(void) NOEXCEPT {

}

void DirectionalLight::RenderUi(void) NOEXCEPT {
  m_color.RenderUi();
  ImGui::DragFloat3("Direction", glm::value_ptr(m_direction), 0.01f, -1.0f, 1.0f, "%.2f");
}

void DirectionalLight::Render(Camera const& camera) NOEXCEPT {
  // Sun
  // Line Position
  // Line Direction
}

PositionalLight::PositionalLight(void) NOEXCEPT {

}

void PositionalLight::RenderUi(void) NOEXCEPT {
  m_color.RenderUi();
  ImGui::DragFloat3("Position", glm::value_ptr(m_position), 0.01f, 0.0f, 0.0f, "%.2f");
}

void PositionalLight::Render(Camera const& camera) NOEXCEPT {
  // Sun
  // Line Position
}

PointLight::PointLight(void) NOEXCEPT {

}

void PointLight::RenderUi(void) NOEXCEPT {
  m_color.RenderUi();
  m_attenuation.RenderUi();
  ImGui::DragFloat3("Position", glm::value_ptr(m_position), 0.01f, 0.0f, 0.0f, "%.2f");
}

void PointLight::Render(Camera const& camera) NOEXCEPT {
  // Point
  // Line Position
}

SpotLight::SpotLight(glm::vec3 const& position) NOEXCEPT
  : m_shaderLine()
  , m_shaderPoint()
  , m_shaderCone()
  , m_shaderCircle()

  , m_innerAngle(glm::pi<float>() / 9.0f)
  , m_outerAngle(glm::pi<float>() / 8.0f)

  , m_color()
  , m_attenuation()
  , m_direction(glm::normalize(-position))
  , m_position(position)
{
  m_shaderLine.SetStart(m_position);
}

void SpotLight::RenderUi(void) NOEXCEPT {
  m_color.RenderUi();
  m_attenuation.RenderUi();

  float degrees[2] = { glm::degrees(m_innerAngle), glm::degrees(m_outerAngle) };
  ImGui::DragFloat2("Angles", degrees, 1.0f, 1.0f, 60.0f, "%.0f");
  float radians[2] = { glm::radians(degrees[0]), glm::radians(degrees[1]) };
  m_innerAngle = radians[0] > m_outerAngle ? m_outerAngle : radians[0];
  m_outerAngle = radians[1] < m_innerAngle ? m_innerAngle : radians[1];

  RenderDirection(m_direction);
  // if (ImGui::DragFloat3("Direction", glm::value_ptr(m_direction), 0.01f, -1.0f, 1.0f, "%.2f")) {
  //   m_direction = glm::normalize(m_direction);
  // }

  if (ImGui::DragFloat3("Position", glm::value_ptr(m_position), 0.01f, 0.0f, 0.0f, "%.2f")) {
    m_shaderLine.SetStart(m_position);
  }
}

void SpotLight::Render(Camera const& camera) NOEXCEPT {
  m_shaderPoint.SetPosition(m_position);
  m_shaderPoint.Render(camera);

  m_shaderLine.SetEnd(glm::vec3(m_position.x, 0.0f, m_position.z));
  m_shaderLine.Render(camera);

  float distance = m_attenuation.GetDistance();
  m_shaderLine.SetEnd(m_position + m_direction * distance * 1.5f);
  m_shaderLine.Render(camera);

  m_shaderCircle.SetInnerRadius(glm::tan(m_innerAngle) * distance);
  m_shaderCircle.SetOuterRadius(glm::tan(m_outerAngle) * distance);
  m_shaderCircle.SetPosition(m_position + m_direction * distance);
  m_shaderCircle.SetDirection(m_direction);
  m_shaderCircle.Render(camera);

  m_shaderCone.SetApex(m_position);
  m_shaderCone.SetBase(m_position + m_direction * distance);
  m_shaderCone.SetRadius(glm::tan(m_outerAngle) * distance);
  m_shaderCone.Render(camera);
}

TR_END_NAMESPACE()
