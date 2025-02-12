#ifndef TR_WINDOW_HPP
#define TR_WINDOW_HPP

#include <glad/glad.h> // glClearColor()
#include <GLFW/glfw3.h> // GLFW Library
#include "imgui/imgui.h" // ImGuiID

#include <cstdio> // fprintf(), stderr
#include <optional> // std::optional{}
#include <utility> // std::in_place

#include "Theme.hpp" // Theme{}
#include "Engine.hpp" // Engine{}
#include "helper.hpp" // TR_DELETE_XXX_CTOR()

///
/// Handle GLFW and Dear ImGui initialisation.
/// Create the Engine.
///
class Window final {
public:
  static std::optional<Window> Create(void) NOEXCEPT;

  static void MouseCallback(GLFWwindow*, double, double) NOEXCEPT;
  static void ScrollCallback(GLFWwindow*, double, double) NOEXCEPT;
  static void KeyboardCallback(GLFWwindow*, int, int, int, int) NOEXCEPT;

public:
   Window(GLFWwindow*) NOEXCEPT;
  ~Window(void) NOEXCEPT;

  bool MainLoop(void) NOEXCEPT;
  void ProcessInput(void) NOEXCEPT;

  void RenderUi(void) NOEXCEPT;
  void RenderEngine(void) NOEXCEPT;

private:
  TR_DELETE_COPY_CTOR(Window);
  TR_DELETE_MOVE_CTOR(Window);

  void ToggleNavigationMode(bool enter) NOEXCEPT;

  double m_currentTime = 0.0;
  double m_elapsedTime = 0.0;

  bool m_navigationMode = false;

  GLFWwindow* m_window;
  ImGuiID m_dockSpaceId;
  Engine m_engine;
  Theme m_theme;
};

#endif // TR_WINDOW_HPP
