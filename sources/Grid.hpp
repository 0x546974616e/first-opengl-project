#ifndef TR_GRID_HPP
#define TR_GRID_HPP

#include <glad/glad.h> // OpenGL

#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

class Grid final {
private:
  enum Flags: GLuint {
    GRID_NONE = 0x0u,
    SHOW_GRID = 0x1u,

    GRID_AXIS_X    = 0x010u,
    GRID_AXIS_Y    = 0x020u,
    GRID_AXIS_Z    = 0x040u,
    GRID_AXIS_MASK = 0x070u,

    GRID_PLANE_XY   = 0x100u,
    GRID_PLANE_YZ   = 0x200u,
    GRID_PLANE_XZ   = 0x400u,
    GRID_PLANE_MASK = 0x700u,
  };

public:
  Grid(void) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;
  void RenderUi(void) NOEXCEPT;

private:
  GLuint m_VAO;
  Shader m_shader;
  Flags m_flags;
};

#endif // TR_GRID_HPP
