#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuiCustom.hpp"

#include <glad/glad.h> // OpenGL API

#include "Grid.hpp" // Grid{}
#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

Grid::Grid(void) NOEXCEPT: m_VAO(0u), m_flags(GRID_NONE) {
  m_flags = static_cast<Flags>(SHOW_GRID | GRID_AXIS_X | GRID_AXIS_Z | GRID_PLANE_XZ);

  glGenVertexArrays(1, &m_VAO);

  m_shader.Attach("grid.vert.glsl");
  m_shader.Attach("grid.frag.glsl");
  m_shader.Link();

  TR_DEBUG("Grid created.");
}

void Grid::RenderUi(void) NOEXCEPT {
  ImU64 show = m_flags & SHOW_GRID;
  ImU64 axis = m_flags & GRID_AXIS_MASK;
  ImU64 plane = m_flags & GRID_PLANE_MASK;

  static char const* axisLabels[3] = { "X", "Y", "Z" };
  static char const* planeLabels[3] = { "XY", "YZ", "XZ" };

  static ImU64 axisFlags[3] = { GRID_AXIS_X, GRID_AXIS_Y, GRID_AXIS_Z };
  static ImU64 planeFlags[3] = { GRID_PLANE_XY, GRID_PLANE_YZ, GRID_PLANE_XZ };

  ImGui::CheckboxFlags("Show Grid", &show, SHOW_GRID);
  ImGui::ButtonFlagsGroup("Axis", &axis, axisFlags, axisLabels, 3);
  ImGui::ButtonFlagsGroup("Plane", &plane, planeFlags, planeLabels, 3, ImGuiButtonFlagsGroup_Exclusive);

  m_flags = static_cast<Flags>(show | axis | plane);
}

// TODO: Render the othogonal axis to the current plane when requested.
void Grid::Render(Camera const& camera) NOEXCEPT {
  if ((m_flags & (GRID_AXIS_MASK | GRID_PLANE_MASK)) == GRID_NONE) return;

  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_projection", camera.Projection());
  m_shader.Bind("tr_camera", camera.Position());
  m_shader.Bind("tr_near", camera.Near());
  m_shader.Bind("tr_far", camera.Far());
  m_shader.Bind("tr_flags", m_flags);

  // Attribute-less rendering.
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
