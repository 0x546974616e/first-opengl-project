#include "imgui/imgui.h"

#include <glad/glad.h> // OpenGL Loader
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp> // glm::mat4{}
#include <glm/vec3.hpp> // glm::vec3{}

#include "Cube.hpp" // Cube{}
#include "Engine.hpp" // Engine{}
#include "helper.hpp" // TR_ARRAYSIZE()

#include "light/Light.hpp" // TMP

TR_BEGIN_NAMESPACE()

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

static SpotLight* s_spot = NULL;

void Engine::Render(Event event) NOEXCEPT {
  for (size_t i = 0u; i < TR_ARRAYSIZE(positions); ++i) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, positions[i]);
    float angle = static_cast<float>(event.currentTime) * 15.0f * static_cast<float>(i+1);
    model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
    m_cube.Transform(model);
    m_cube.Render(m_camera);
  }

  if (s_spot != NULL) s_spot->Render(m_camera);

/*
  m_light.SetPosition(glm::vec3(1,2,3));
  m_light3.SetStart(glm::vec3(1,2,3));
  m_light3.SetEnd(glm::vec3(1,0,3));

  m_light.Render(m_camera);
  m_light3.Render(m_camera);

  m_light2.SetPosition(glm::vec3(1,2,4));
  m_light3.SetStart(glm::vec3(1,2,4));
  m_light3.SetEnd(glm::vec3(1,0,4));

  m_light2.Render(m_camera);
  m_light3.Render(m_camera);

  {
    glm::vec3 p(1, 2, -4);
    // glm::vec3 d(4, 2, -4);
    glm::vec3 d(-2, 3, -4);

    m_light3.SetStart(p);
    m_light3.SetEnd(p + d);
    m_light3.Render(m_camera);

    float outerRadius = 2.0f;
    float innerRadius = 1.8f;

    m_light4.SetInnerRadius(innerRadius);
    m_light4.SetOuterRadius(outerRadius);

    m_light4.SetPosition(p);
    m_light4.SetDirection(d);
    m_light4.Render(m_camera);

    m_lightCone.SetRadius(outerRadius);
    m_lightCone.SetApex(p + d);
    m_lightCone.SetBase(p);
    m_lightCone.Render(m_camera);
  }

  {
    m_light5.SetPosition({ -1, -2, 3 });
    m_light5.Render(m_camera);
  }
 */

  m_grid.Render(m_camera);
}

void Engine::RenderUi(void) NOEXCEPT {
  ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen;

  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (ImGui::TreeNode("Camera")) {
    m_camera.RenderUi();
    ImGui::TreePop();
  }

  ImGui::SetNextItemOpen(true, ImGuiCond_Once);
  if (ImGui::TreeNode("Grid")) {
    m_grid.RenderUi();
    ImGui::TreePop();
  }

  ImGui::Indent();
  if (s_spot == NULL) s_spot = new SpotLight({4, 5, 6});
  s_spot->RenderUi();
  ImGui::Unindent();
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

TR_END_NAMESPACE()
