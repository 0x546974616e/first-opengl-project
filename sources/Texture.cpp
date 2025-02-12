#include <glad/glad.h> // OpenGL Loader
#include <stb/stb_image.h> // stbi_load()

#include <string> // std::string{}
#include <string_view> // std::string_view{}

#include "helper.hpp" // TR_ERROR()
#include "Texture.hpp" // Self{}

Texture::Texture(std::string_view filename) noexcept {
  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  // Set the texture wrapping parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set texture filtering parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  stbi_set_flip_vertically_on_load(true); // Flip Y-axis.
  std::string path = TR_RESOURCES_DIR "/textures/"; path += filename;
  unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 0);

  if (data != NULL) {
    if (channels == 3 || channels == 4) {
      GLint format = channels == 3 ? GL_RGB : GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, (GLenum) format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      TR_DEBUG("Unsupported %d-channel: %s", channels, filename.data());
    }
  }
  else {
    TR_DEBUG("Failed to load texture: %s", filename.data());
  }

  stbi_image_free(data);
}
