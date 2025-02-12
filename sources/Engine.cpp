#include <glad/glad.h> // OpenGL Loader
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp> // glm::mat4{}
#include <glm/vec3.hpp> // glm::vec3{}

#include "Cube.hpp" // Cube{}
#include "Engine.hpp" // Engine{}
#include "helper.hpp" // TR_ARRAYSIZE()

#include <cstdio> // fprintf(), stdout

static glm::vec3 positions[] = {
  glm::vec3( 0.0f, 0.0f, 0.0f),
  glm::vec3( 2.0f, 5.0f, -15.0f),
  glm::vec3(-1.5f, -2.2f, -2.5f),
  glm::vec3(-3.8f, -2.0f, -12.3f),
  glm::vec3( 2.4f, -0.4f, -3.5f),
  glm::vec3(-1.7f, 3.0f, -7.5f),
  glm::vec3( 1.3f, -2.0f, -2.5f),
  glm::vec3( 1.5f, 2.0f, -2.5f),
  glm::vec3( 1.5f, 0.2f, -1.5f),
  glm::vec3(-1.3f, 1.0f, -1.5f)
};

void Engine::Render(Event event) NOEXCEPT {
  // TR_DEBUG("Array Size: %zu", TR_ARRAYSIZE(positions));
  for (size_t i = 0u; i < TR_ARRAYSIZE(positions); ++i) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, positions[i]);
    float angle = static_cast<float>(event.currentTime) * 15.0f * static_cast<float>(i+1);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    m_cube.Transform(model);
    m_cube.Render(m_camera);
  }
}

void Engine::ProcessMouse(MouseEvent event) NOEXCEPT {
  m_camera.ProcessMouse(event);
}

void Engine::ProcessScroll(ScrollEvent event) NOEXCEPT {
  m_camera.ProcessScroll(event);
}

void Engine::ProcessKeyboard(KeyboardEvent event) NOEXCEPT {
  m_camera.ProcessKeyboard(event);
}

void Engine::Focus(void) NOEXCEPT {
  m_camera.Focus();
}

void Engine::UnFocus(void) NOEXCEPT {
  m_camera.UnFocus();
}
