#include <glad/glad.h> // OpenGL API

#include "Grid.hpp" // Grid{}
#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

static char const* GridVertexShader = R"(
  #version 330 core

  out vec2 uv;

  uniform mat4 view;
  uniform mat4 projection;
  uniform vec3 cameraPosition;

  const vec3 positions[4] = vec3[4](
    vec3(-1.0, 0.0, -1.0), // Bottom Left
    vec3(+1.0, 0.0, -1.0), // Bottom Right
    vec3(+1.0, 0.0, +1.0), // Top Right
    vec3(-1.0, 0.0, +1.0)  // Top Left
  );

  const int indices[6] = int[6](0, 2, 1, 2, 0, 3);

  void main() {
    int index = indices[gl_VertexID];
    uv = vec2(positions[index].xz);
    vec3 position = positions[index] * 10.0;
    position.x += cameraPosition.x * 0.00000001;
    position.z += cameraPosition.z * 0.00000001;
    gl_Position = projection * view * vec4(position, 1.0);
  }
)";

static char const* GridFragmentShader = R"(
  #version 330 core
  in vec2 uv;

  layout (location = 0) out vec4 tr_Fragment;
  void main() {
    vec2 dada = vec2(uv * 2);
    tr_Fragment = vec4(dada, 0.0, 0.2);
  }
)";

Grid::Grid(void) NOEXCEPT: m_VAO(0u) {
  glGenVertexArrays(1, &m_VAO);
  m_shader.Attach(GL_VERTEX_SHADER, GridVertexShader);
  m_shader.Attach(GL_FRAGMENT_SHADER, GridFragmentShader);
  m_shader.Link();
  TR_DEBUG("Grid created.");
}

void Grid::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);
  m_shader.Bind("view", camera.LookAt());
  m_shader.Bind("projection", camera.Projection());
  m_shader.Bind("cameraPosition", camera.Position());
  // Attribute-less rendering.
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}
