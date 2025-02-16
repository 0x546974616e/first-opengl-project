#ifndef TR_CUBE_HPP
#define TR_CUBE_HPP

#include <glad/glad.h> // OpenGL API
#include <glm/mat4x4.hpp>

#include "Camera.hpp" // Camera{}
#include "Texture.hpp" // Texture{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

TR_BEGIN_NAMESPACE()

class Cube final {
public:
  Cube(void) noexcept;

  void Render(Camera const& camera) NOEXCEPT;

  constexpr void Transform(glm::mat4 const& transform) NOEXCEPT {
    m_model = transform;
  }

private:
  glm::mat4 m_model = { 1.0f };

  GLuint m_VAO; // Vertex Array Object
  GLuint m_VBO; // Vertex Buffer Object

  Texture m_texture1;
  Texture m_texture2;

  Shader m_shader;
};

TR_END_NAMESPACE()

#endif // TR_CUBE_HPP
