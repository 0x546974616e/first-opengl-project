#ifndef TR_LIGHT_HPP
#define TR_LIGHT_HPP

#include <glad/glad.h> // OpenGL
#include <glm/vec3.hpp> // glm::vec3{}

#include "Camera.hpp" // Camera{}
#include "Shader.hpp" // Shader{}
#include "helper.hpp" // NOEXCEPT

#include "light/LightColor.hpp"
#include "light/LightAttenuation.hpp"
#include "light/LightShader.hpp"

#define TR_LIGHT_TYPE(NAME, PREFIX) \
  enum NAME {          \
    DIRECTIONAL_LIGHT, \
    POSITIONAL_LIGHT,  \
    POINT_LIGHT,       \
    SPOT_LIGHT,        \
  }

#define TR_LIGHT_STRUCT(NAME, UINT, VEC3) \
  struct NAME {       \
    UINT type;        \
    VEC3 position;    \
    VEC3 direction;   \
    VEC3 attenuation; \
    VEC3 angle;       \
    VEC3 ambient;     \
    VEC3 diffuse;     \
    VEC3 specular;    \
  };

#define TR_LIGHT_CPP()  TR_LIGHT(LightGlsl, GLuint, glm::vec3)
#define TR_LIGHT_GLSL() TR_LIGHT(Light, uint, vec3)

TR_BEGIN_NAMESPACE()

class DirectionalLight final {
public:
  DirectionalLight(void) NOEXCEPT;

  void RenderUi(void) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;

private:
  LightShaderLine m_shaderLine;
  LightShaderSun m_shaderSun;

  LightColor m_color;
  glm::vec3 m_direction;
};

class PositionalLight final {
public:
  PositionalLight(void) NOEXCEPT;

  void RenderUi(void) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;

private:
  LightShaderLine m_shaderLine;
  LightShaderSun m_shaderSun;

  LightColor m_color;
  glm::vec3 m_position;
};

class PointLight final {
public:
  PointLight(void) NOEXCEPT;

  void RenderUi(void) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;

private:
  LightShaderLine m_shaderLine;
  LightShaderPoint m_shaderPoint;
  LightShaderFaceCircle m_shaderCircle;

  LightColor m_color;
  LightAttenuation m_attenuation;
  glm::vec3 m_position;
};

class SpotLight final {
public:
  SpotLight(glm::vec3 const& position = {}) NOEXCEPT;

  void RenderUi(void) NOEXCEPT;
  void Render(Camera const& camera) NOEXCEPT;

private:
  LightShaderLine m_shaderLine;
  LightShaderPoint m_shaderPoint;
  LightShaderConeLine m_shaderCone;
  LightShaderRangeCircle m_shaderCircle;

  float m_innerAngle;
  float m_outerAngle;
  LightColor m_color;
  LightAttenuation m_attenuation;
  glm::vec3 m_direction;
  glm::vec3 m_position;
};

TR_END_NAMESPACE()

#endif // TR_LIGHT_HPP
