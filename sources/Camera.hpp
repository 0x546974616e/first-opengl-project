#ifndef TR_CAMERA_HPP
#define TR_CAMERA_HPP

#include <glm/gtc/matrix_transform.hpp> // glm::lookAt()
#include <glm/mat4x4.hpp> // glm::mat4{}

#include "helper.hpp" // NOEXCEPT
#include "Event.hpp" // Event{}

TR_BEGIN_NAMESPACE()

class Camera final {
public:
  void RenderUi(void) NOEXCEPT;

  void ProcessMouse(MouseEvent event) NOEXCEPT;
  void ProcessScroll(ScrollEvent event) NOEXCEPT;
  void ProcessKeyboard(KeyboardEvent event) NOEXCEPT;

  constexpr void Focus(void) NOEXCEPT { m_firstMouse = true; }
  constexpr void UnFocus(void) NOEXCEPT { m_firstMouse = true; }

  constexpr void SetDimensions(int width, int height) NOEXCEPT {
    m_width = width; m_height = height;
  }

public:
  constexpr float AspectRatio(void) const NOEXCEPT {
    return static_cast<float>(m_width) / static_cast<float>(m_height);
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
      glm::radians(m_fov),
      AspectRatio(),
      m_near, m_far
    );
  }

public:
  constexpr glm::vec3 Position(void) const NOEXCEPT { return m_position; }

  constexpr int Width(void) const NOEXCEPT { return m_width; }
  constexpr int Height(void) const NOEXCEPT { return m_height; }

  constexpr float Near(void) const NOEXCEPT { return m_near; }
  constexpr float Far(void) const NOEXCEPT { return m_far; }

protected:
  int m_width = 0;
  int m_height = 0;

  float m_near = 0.1f;
  float m_far = 100.0f;

  bool m_firstMouse = true;
  MouseEvent m_lastMouse;

  glm::vec3 m_position = { 0.0, 1.0, 3.0 };
  glm::vec3 m_target = { 0.0, 0.0, -1.0 };
  glm::vec3 m_cross = { 0.0, 1.0, 0.0 };

  float m_fov = 50.0f;
  float m_speed = 5.0f;
  float m_sensitivity = 0.1f;
  float m_pitch = 0.0f;
  float m_yaw = -90.0f;
};

TR_END_NAMESPACE()

#endif // TR_CAMERA_HPP
