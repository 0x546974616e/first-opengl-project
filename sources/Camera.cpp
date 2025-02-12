#include <glm/vec3.hpp> // glm::vec3{}

#include "Camera.hpp" // Self{}
#include "helper.hpp" // NOEXCEPT

void Camera::ProcessMouse(MouseEvent event) NOEXCEPT {
  if (m_firstMouse) {
    m_lastMouse = event;
    m_firstMouse = false;
  }

  double xOffset = event.x - m_lastMouse.x;
  double yOffset = m_lastMouse.y - event.y;
  m_lastMouse = event;

  xOffset *= m_sensitivity;
  yOffset *= m_sensitivity;

  m_yaw += xOffset;
  m_pitch += yOffset;
  m_pitch = TR_CLAMP(m_pitch, -89.0, 89.0);

  glm::vec<3, double> front;
  front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  front.y = sin(glm::radians(m_pitch));
  front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
  front = glm::normalize(front);

  m_target.x = static_cast<float>(front.x);
  m_target.y = static_cast<float>(front.y);
  m_target.z = static_cast<float>(front.z);
}

void Camera::ProcessScroll(ScrollEvent event) NOEXCEPT {
  m_fov -= event.yOffset;
  m_fov = TR_CLAMP(m_fov, 1.0, 45.0);
}

void Camera::ProcessKeyboard(KeyboardEvent event) NOEXCEPT {
  float cameraSpeed = static_cast<float>(2.5 * event.elapsedTime);

  if (event.keyW) m_position += cameraSpeed * m_target;
  if (event.keyS) m_position -= cameraSpeed * m_target;

  if (event.shift) m_position -= cameraSpeed * m_cross;
  if (event.space) m_position += cameraSpeed * m_cross;

  if (event.keyA) m_position -= glm::normalize(glm::cross(m_target, m_cross)) * cameraSpeed;
  if (event.keyD) m_position += glm::normalize(glm::cross(m_target, m_cross)) * cameraSpeed;
}
