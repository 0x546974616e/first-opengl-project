#ifndef TR_TEXTURE_HPP
#define TR_TEXTURE_HPP

#include <glad/glad.h> // OpenGL Loader
#include <string_view> // std::string_view{}

TR_BEGIN_NAMESPACE()

class Texture {
public:
  Texture(std::string_view filename) noexcept;

  constexpr operator GLuint(void) noexcept {
    return m_texture;
  }

  constexpr GLuint getTexture(void) noexcept {
    return m_texture;
  }

private:
  GLuint m_texture;
};

TR_END_NAMESPACE()

#endif // TR_TEXTURE_HPP
