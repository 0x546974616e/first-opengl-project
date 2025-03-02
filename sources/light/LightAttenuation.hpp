#ifndef TR_LIGHT_ATTENUATION_HPP
#define TR_LIGHT_ATTENUATION_HPP

#include "helper.hpp" // NOEXCEPT, NAMESPACE

TR_BEGIN_NAMESPACE()

class LightAttenuation final {
public:
  void RenderUi(void) NOEXCEPT;
  constexpr LightAttenuation(float distance = 7.0f) NOEXCEPT
    : m_useTerms(false), m_strength(0.0f), m_distance(distance)
  { }

  constexpr float GetDistance(void) const NOEXCEPT {
    return m_distance;
  }

private:
  static constexpr float MIN_ATTENUATION = 0.05f;

  float ComputeMaxStrength(void) NOEXCEPT;
  float ComputeAttenuation(float distance) NOEXCEPT;
  float FindDistance(float linear, float quadratic) NOEXCEPT;
  void RenderSliders(bool more = false) NOEXCEPT;

private:
  bool m_useTerms;

  float m_strength;
  float m_distance;
  float m_constant = 1.0f;
};

TR_END_NAMESPACE()

#endif // TR_LIGHT_ATTENUATION_HPP
