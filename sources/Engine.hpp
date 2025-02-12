#ifndef TR_ENGINE_HPP
#define TR_ENGINE_HPP

#include "helper.hpp" // NOEXCEPT
#include "Camera.hpp" // Camera{}
#include "Event.hpp" // Event{}

#include "Cube.hpp" // Cube{}
#include "Grid.hpp" // Grid{}

class Engine final {
public:
  void Render(Event event) NOEXCEPT;

  void ProcessMouse(MouseEvent event) NOEXCEPT;
  void ProcessScroll(ScrollEvent event) NOEXCEPT;
  void ProcessKeyboard(KeyboardEvent event) NOEXCEPT;

  void Focus(void) NOEXCEPT;
  void UnFocus(void) NOEXCEPT;

  constexpr void SetViewport(int width, int height) NOEXCEPT {
    m_camera.SetDimensions(width, height);
  }

private:
  Camera m_camera{};

  Cube m_cube{};
  Grid m_grid{};
};

#endif // TR_ENGINE_HPP
