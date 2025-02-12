#ifndef TR_GRID_HPP
#define TR_GRID_HPP

#include <glad/glad.h> // OpenGL

#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

class Grid final {
public:
  Grid(void) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;

private:
  GLuint m_VAO;
  Shader m_shader;
};

#endif // TR_GRID_HPP
