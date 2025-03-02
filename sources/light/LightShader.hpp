#ifndef TR_LIGHT_SHADER
#define TR_LIGHT_SHADER

#include <glad/glad.h> // OpenGL
#include <glm/vec3.hpp>

#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp"

TR_BEGIN_NAMESPACE()

class LightShaderSun final {
public:
  LightShaderSun(glm::vec3 const& position = glm::vec3{}) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;
  constexpr void SetPosition(glm::vec3 const& position) NOEXCEPT {
    m_position = position;
  }

private:
  glm::vec3 m_position;
  Shader m_shader;
  GLuint m_VAO;
};

class LightShaderPoint final {
public:
  LightShaderPoint(glm::vec3 const& position = glm::vec3{}) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;
  constexpr void SetPosition(glm::vec3 const& position) NOEXCEPT {
    m_position = position;
  }

private:
  glm::vec3 m_position;
  Shader m_shader;
  GLuint m_VAO;
};

class LightShaderLine final {
public:
  LightShaderLine(
    glm::vec3 const& start = glm::vec3{},
    glm::vec3 const& end = glm::vec3{}
  ) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;

  constexpr void SetStart(glm::vec3 const& start) NOEXCEPT { m_start = start; }
  constexpr void SetEnd(glm::vec3 const& end) NOEXCEPT { m_end = end; }

private:
  glm::vec3 m_start, m_end;
  Shader m_shader;
  GLuint m_VAO;
};

class LightShaderConeLine final {
public:
  LightShaderConeLine(
    glm::vec3 const& apex = glm::vec3{},
    glm::vec3 const& base = glm::vec3{},
    float radius = 1.0f
  ) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;

  constexpr void SetApex(glm::vec3 const& apex) NOEXCEPT { m_apex = apex; }
  constexpr void SetBase(glm::vec3 const& base) NOEXCEPT { m_base = base; }
  constexpr void SetRadius(float radius) NOEXCEPT { m_radius = radius; }

private:
  float m_radius;
  glm::vec3 m_apex;
  glm::vec3 m_base;
  Shader m_shader;
  GLuint m_VAO;
};

class LightShaderRangeCircle final {
public:
  LightShaderRangeCircle(
    glm::vec3 const& position = glm::vec3{},
    glm::vec3 const& direction = glm::vec3{},
    float innerRadius = 1,
    float outerRadius = 2
  ) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;

  constexpr void SetPosition(glm::vec3 const& position) NOEXCEPT { m_position = position; }
  constexpr void SetDirection(glm::vec3 const& direction) NOEXCEPT { m_direction = direction; }

  constexpr void SetInnerRadius(float radius) NOEXCEPT { m_innerRadius = radius; }
  constexpr void SetOuterRadius(float radius) NOEXCEPT { m_outerRadius = radius; }

private:
  glm::vec3 m_position;
  glm::vec3 m_direction;
  float m_innerRadius;
  float m_outerRadius;
  Shader m_shader;
  GLuint m_VAO;
};

class LightShaderFaceCircle final {
public:
  LightShaderFaceCircle(
    glm::vec3 const& position = glm::vec3{},
    float radius = 1.0f
  ) NOEXCEPT;

  void Render(Camera const& camera) NOEXCEPT;

  constexpr void SetPosition(glm::vec3 const& position) NOEXCEPT { m_position = position; }
  constexpr void SetRadius(float radius) NOEXCEPT { m_radius = radius; }

private:
  float m_radius;
  glm::vec3 m_position;
  Shader m_shader;
  GLuint m_VAO;
};

TR_END_NAMESPACE()

#endif // TR_LIGHT_SHADER
