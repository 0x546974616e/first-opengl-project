#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuiCustom.hpp"

#include <glad/glad.h> // OpenGL API
#include <glm/gtc/type_ptr.hpp> // glm::make_vec4()

#include "Grid.hpp" // Grid{}
#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

TR_BEGIN_NAMESPACE()

Grid::Grid(void) NOEXCEPT
  : m_VAO(0u), m_flags(GRID_NONE), m_lineSize(0.2f)
{
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

  ImGui::DragFloat("Line Size", &m_lineSize, 0.01f, 0.01f, 2.0f, "%.1f");
  ImGui::ButtonFlagsGroup("Axis", &axis, axisFlags, axisLabels, 3);
  ImGui::ButtonFlagsGroup("Floor", &plane, planeFlags, planeLabels, 3, ImGuiButtonFlagsGroup_Exclusive);
  ImGui::CheckboxFlags("Show Grid", &show, SHOW_GRID);

  m_flags = static_cast<Flags>(show | axis | plane);
}

void Grid::OnThemeUpdate(Theme& theme) NOEXCEPT {
  m_shader.Use();
  // TODO: Probably temporary
  ImVec4 colorGrid         = theme.Get(Theme::ColorGrid);
  ImVec4 colorGridEmphasis = theme.Get(Theme::ColorGridEmphasis);

  m_shader.Bind("tr_colorGrid"        , glm::make_vec4(&colorGrid.x));
  m_shader.Bind("tr_colorGridEmphasis", glm::make_vec4(&colorGridEmphasis.x));

  ImVec4 colorAxisX = theme.Get(Theme::ColorAxisX);
  ImVec4 colorAxisY = theme.Get(Theme::ColorAxisY);
  ImVec4 colorAxisZ = theme.Get(Theme::ColorAxisZ);

  m_shader.Bind("tr_colorGridAxisX", glm::make_vec4(&colorAxisX.x));
  m_shader.Bind("tr_colorGridAxisY", glm::make_vec4(&colorAxisY.x));
  m_shader.Bind("tr_colorGridAxisZ", glm::make_vec4(&colorAxisZ.x));
}

// TODO: Render the othogonal axis to the current plane when requested (glDepthFunc(GL_ALWAYS)?)
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
  m_shader.Bind("tr_lineSize", m_lineSize);

  // Attribute-less rendering.
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

TR_END_NAMESPACE()
