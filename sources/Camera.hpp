#ifndef TR_CAMERA_HPP
#define TR_CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp> // glm::lookAt()
#include <glm/mat4x4.hpp> // glm::mat4{}

#include "helper.hpp" // NOEXCEPT
#include "Event.hpp" // Event{}

class Camera final {
public:
  void ProcessMouse(MouseEvent event) NOEXCEPT;
  void ProcessScroll(ScrollEvent event) NOEXCEPT;
  void ProcessKeyboard(KeyboardEvent event) NOEXCEPT;

  constexpr void Focus(void) NOEXCEPT { m_firstMouse = true; }
  constexpr void UnFocus(void) NOEXCEPT { m_firstMouse = true; }

  constexpr void SetDimensions(int width, int height) NOEXCEPT {
    m_width = width; m_height = height;
  }

  constexpr float AspectRatio(void) const NOEXCEPT {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
  }

  constexpr glm::vec3 Position(void) const NOEXCEPT {
    return m_position;
  }

  constexpr glm::mat4 LookAt(void) const NOEXCEPT {
    return glm::lookAt(
      m_position, // Eye/Position
      m_position + m_target, // Target/Center
      m_cross // Up/CrossProduct
    );
  }

  constexpr glm::mat4 Projection(void) const NOEXCEPT {
    return glm::perspective(
      glm::radians(static_cast<float>(m_fov)),
      AspectRatio(),
      0.1f, // Near
      100.0f // Far
    );
  }

protected:
  int m_width = 0;
  int m_height = 0;

  bool m_firstMouse = true;
  MouseEvent m_lastMouse;

  glm::vec3 m_position = { 0.0, 1.0, 3.0 };
  glm::vec3 m_target = { 0.0, 0.0, -1.0 };
  glm::vec3 m_cross = { 0.0, 1.0, 0.0 };

  double m_fov = 45.0;
  double m_sensitivity = 0.1;
  double m_pitch = 0.0;
  double m_yaw = -90.0;
};

#endif // TR_CAMERA_HPP
