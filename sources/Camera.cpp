#include "imgui/imgui.h"
#include <glm/vec3.hpp> // glm::vec3{}
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr{}

#include "Camera.hpp" // Self{}
#include "helper.hpp" // NOEXCEPT
#include "Log.hpp"

TR_BEGIN_NAMESPACE()

void Camera::RenderUi(void) NOEXCEPT {
  bool update = false;
  ImGui::SeparatorText("Controls");

  ImGui::DragFloat("FOV", &m_fov, 1.0f, 1.0f, 89.0f, "%.0f");
  ImGui::DragFloat("Speed", &m_speed, 0.1f, 0.1f, 50.0f, "%.1f");

  float planes[2] = { m_near, m_far };
  ImGui::DragFloat2("Near/Far", planes, 0.1f, 0.1f, 200.f, "%.1f");
  m_near = TR_MIN(planes[0], planes[1]);
  m_far = TR_MAX(planes[0], planes[1]);

  ImGui::SeparatorText("Transformations");

  float rotations[2] = { m_pitch, m_yaw };
  update = ImGui::DragFloat2("Pitch/Yaw", rotations, 1.0f, 0.0f, 0.0f, "%.0f");
  m_pitch = TR_CLAMP(rotations[0], -89.0f, 89.0f);
  m_yaw = rotations[1];

  ImGui::DragFloat3("XYZ", glm::value_ptr(m_position), 0.01f, 0.0f, 0.0f, "%.2f");

  if (update) {
    glm::vec3 front;
    front.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    front.y = sinf(glm::radians(m_pitch));
    front.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
    m_target = glm::normalize(front);
  }
}

void Camera::ProcessMouse(MouseEvent event) NOEXCEPT {
  if (m_firstMouse) {
    m_lastMouse = event;
    m_firstMouse = false;
  }

  float xOffset = static_cast<float>(event.x - m_lastMouse.x);
  float yOffset = static_cast<float>(m_lastMouse.y - event.y);
  m_lastMouse = event;

  xOffset *= m_sensitivity;
  yOffset *= m_sensitivity;

  m_yaw += xOffset;
  m_pitch += yOffset;
  m_pitch = TR_CLAMP(m_pitch, -89.0f, 89.0f);

  glm::vec3 front;
  front.x = cosf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
  front.y = sinf(glm::radians(m_pitch));
  front.z = sinf(glm::radians(m_yaw)) * cosf(glm::radians(m_pitch));
  m_target = glm::normalize(front);
}

void Camera::ProcessScroll(ScrollEvent event) NOEXCEPT {
  m_fov -= static_cast<float>(event.yOffset);
  m_fov = TR_CLAMP(m_fov, 1.0, 89.0);
}

void Camera::ProcessKeyboard(KeyboardEvent event) NOEXCEPT {
  float cameraSpeed = m_speed * static_cast<float>(event.elapsedTime);

  // if (event.keyW) m_position += cameraSpeed * m_target;
  // if (event.keyS) m_position -= cameraSpeed * m_target;

  if (event.keyW) m_position -= glm::normalize(glm::cross(glm::cross(m_target, m_cross), m_cross)) * cameraSpeed;
  if (event.keyS) m_position += glm::normalize(glm::cross(glm::cross(m_target, m_cross), m_cross)) * cameraSpeed;

  if (event.shift) m_position -= cameraSpeed * m_cross;
  if (event.space) m_position += cameraSpeed * m_cross;

  if (event.keyA) m_position -= glm::normalize(glm::cross(m_target, m_cross)) * cameraSpeed;
  if (event.keyD) m_position += glm::normalize(glm::cross(m_target, m_cross)) * cameraSpeed;
}

TR_END_NAMESPACE()
