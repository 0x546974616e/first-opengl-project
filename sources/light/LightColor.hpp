#ifndef TR_LIGHT_COLOR_HPP
#define TR_LIGHT_COLOR_HPP

#include "imgui/imgui.h"
#include "helper.hpp" // NOEXCEPT

TR_BEGIN_NAMESPACE()

class LightColor final {
public:
  void RenderUi(void) NOEXCEPT;
  constexpr LightColor(void) NOEXCEPT: LightColor(1.0, 1.0, 1.0) {}
  constexpr LightColor(float r, float g, float b) NOEXCEPT: m_specular(r, g, b, 1.0) {
    m_diffuse.w = m_ambient.w = m_specular.w;
    UpdateFromSpecular();
  }

private:
  void UpdateFromSpecular(void) NOEXCEPT;
  void UpdateFromDiffuse(void) NOEXCEPT;
  void UpdateFromAmbient(void) NOEXCEPT;
  void UpdateDiffuse(void) NOEXCEPT;
  void UpdateAmbient(void) NOEXCEPT;

private:
  bool m_bindColors = true;

  float m_diffuseFactor = 0.8f;
  float m_ambientFactor = 0.1f;

  ImVec4 m_specular;
  ImVec4 m_diffuse;
  ImVec4 m_ambient;
};

TR_END_NAMESPACE()

#endif // TR_LIGHT_COLOR_HPP
