#ifndef TR_ENGINE_HPP
#define TR_ENGINE_HPP

#include "helper.hpp" // NOEXCEPT
#include "Camera.hpp" // Camera{}
#include "Event.hpp" // Event{}
#include "Theme.hpp" // Theme{}

#include "Cube.hpp" // Cube{}
#include "Grid.hpp" // Grid{}

TR_BEGIN_NAMESPACE()

class Engine final {
public:
  void Render(Event event) NOEXCEPT;
  void RenderUi(void) NOEXCEPT;

  void ProcessMouse(MouseEvent event) NOEXCEPT;
  void ProcessScroll(ScrollEvent event) NOEXCEPT;
  void ProcessKeyboard(KeyboardEvent event) NOEXCEPT;

  void Focus(void) NOEXCEPT;
  void UnFocus(void) NOEXCEPT;

  constexpr void OnThemeUpdate(Theme& theme) NOEXCEPT {
    m_grid.OnThemeUpdate(theme);
  }

  constexpr void SetViewport(int width, int height) NOEXCEPT {
    m_camera.SetDimensions(width, height);
  }

private:
  Camera m_camera{};

  Cube m_cube{};
  Grid m_grid{};
};

TR_END_NAMESPACE()

#endif // TR_ENGINE_HPP
