#include <glad/glad.h> // OpenGL API
#include <glm/mat4x4.hpp>

#include "Cube.hpp" // Cube{}
#include "Camera.hpp" // Camera{}
#include "Texture.hpp" // Texture{}
#include "Shader.hpp" // Shader{}

#define POS(X, Y, Z) (X), (Y), (Z)
#define RGB(R, G, B) (R), (G), (B)
#define UV(U, V) (U), (V)

#define LOCATION0 0 // GLSL: layout (location = 0)
#define LOCATION1 1 // GLSL: layout (location = 1)
#define LOCATION2 2 // GLSL: layout (location = 2)

#define TEXTURE_UNIT0 0
#define TEXTURE_UNIT1 1
#define TEXTURE_UNIT2 2

static char const* CubeVertexShader = R"(
  #version 330 core
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 color;
  layout (location = 2) in vec2 uv;
  out vec3 tr_Color;
  out vec2 tr_Texture;
  uniform mat4 model;
  uniform mat4 view;
  uniform mat4 projection;
  void main() {
    gl_Position = projection * view * model * vec4(position, 1.0f);
    tr_Color = color;
    tr_Texture = uv;
  }
)";

static char const* CubeFragmentShader = R"(
  #version 330 core
  in vec2 tr_Texture;
  in vec3 tr_Color;
  out vec4 tr_Fragment;
  uniform sampler2D texture1;
  uniform sampler2D texture2;
  void main() {
    vec4 color = mix(
      mix(
        texture(texture1, tr_Texture),
        texture(texture2, tr_Texture),
        0.2f
      ),
      vec4(tr_Color, 1.0f),
      0.2f
    );
    tr_Fragment = vec4(color.xyz, 1.0f);
  }
)";

static GLfloat CubeVertices[] = {
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(0.0f, 0.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(1.0f, 1.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(0.0f, 0.0f),

  POS(-0.5f, -0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(0.0f, 0.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(1.0f, 1.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(0.0f, 0.0f),

  POS(-0.5f, +0.5f, +0.5f), RGB(1.0f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(1.0f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 1.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(0.0f, 1.0f, 1.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(0.0f, 1.0f, 1.0f), UV(0.0f, 0.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(0.0f, 1.0f, 1.0f), UV(1.0f, 0.0f),

  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 0.5f, 0.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 0.0f), UV(1.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(1.0f, 0.5f, 0.0f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(0.5f, 1.0f, 0.5f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.5f, 1.0f, 0.5f), UV(0.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 0.5f), UV(1.0f, 0.0f),

  POS(-0.5f, -0.5f, -0.5f), RGB(0.5f, 0.0f, 1.0f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(0.5f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.5f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.1f), UV(1.0f, 0.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.1f), UV(0.0f, 0.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 1.0f, 0.1f), UV(0.0f, 1.0f),

  POS(-0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 1.0f), UV(0.0f, 1.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 0.5f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 1.0f), UV(1.0f, 0.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 1.0f), UV(0.0f, 0.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(0.5f, 1.0f, 1.0f), UV(0.0f, 1.0f),
};

Cube::Cube(void) noexcept
  : m_texture1("/container.jpg")
  , m_texture2("/awesomeface.png")
{
  m_shader.Attach(GL_VERTEX_SHADER, CubeVertexShader);
  m_shader.Attach(GL_FRAGMENT_SHADER, CubeFragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

  glVertexAttribPointer(LOCATION0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
  glVertexAttribPointer(LOCATION1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
  glVertexAttribPointer(LOCATION2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(LOCATION0);
  glEnableVertexAttribArray(LOCATION1);
  glEnableVertexAttribArray(LOCATION2);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  m_shader.Use();
  m_shader.Bind("texture1", TEXTURE_UNIT0);
  m_shader.Bind("texture2", TEXTURE_UNIT1);

  TR_DEBUG("Cube created.");
}

void Cube::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  // Texture unit = texture location
  glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, m_texture1);
  glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, m_texture2);

  glBindVertexArray(m_VAO);
  m_shader.Bind("model", m_model);
  m_shader.Bind("view", camera.LookAt());
  m_shader.Bind("projection", camera.Projection());
  glDrawArrays(GL_TRIANGLES, 0, 36);
  glBindVertexArray(0);
}
