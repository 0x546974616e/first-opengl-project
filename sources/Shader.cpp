#include <glad/glad.h> // OpenGL API

#include <cstdio> // fprintf(), stderr
#include <memory> // std::unique_ptr{}

#include <fstream> // std::ifstream{}
#include <sstream> // std::stringstream{}
#include <string> // std::string{}

#include "helper.hpp" // TR_ERROR()
#include "Shader.hpp" // Self{}

void Shader::Attach(GLenum type, std::string_view source) noexcept {
  GLint status, length;
  GLuint shader = glCreateShader(type);
  char const* data = source.data();
  glShaderSource(shader, 1, &data, NULL);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> buffer(new char[static_cast<size_t>(length)]);
    glGetShaderInfoLog(shader, length, NULL, buffer.get());
    fprintf(stderr, "%s", buffer.get());
  }

  // https://gamedev.stackexchange.com/questions/47910/after-a-succesful-gllinkprogram-should-i-delete-detach-my-shaders
  glAttachShader(m_program, shader);
  glDeleteShader(shader);
}

void Shader::Attach(std::string_view filename) noexcept {
  size_t index = filename.rfind(".");
  if (index == std::string_view::npos) {
    TR_ERROR("Shader extension is missing: %s", filename.data());
    return;
  }

  GLenum type;
  std::string_view extension = filename.substr(index + 1);
       if (extension == "comp") type = GL_COMPUTE_SHADER;
  else if (extension == "frag") type = GL_FRAGMENT_SHADER;
  else if (extension == "geom") type = GL_GEOMETRY_SHADER;
  else if (extension == "vert") type = GL_VERTEX_SHADER;
  else {
    TR_ERROR("Unknown shader extension: %s", filename.data());
    return;
  }

  std::string path = TR_RESOURCES_DIR "/shaders/";
  std::ifstream file(path += filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  Attach(type, source);
}

void Shader::Link(void) noexcept {
  GLint status, length;
  glLinkProgram(m_program);

  glGetProgramiv(m_program, GL_LINK_STATUS, &status);
  if(status == GL_FALSE) {
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> buffer(new char[static_cast<size_t>(length)]);
    glGetProgramInfoLog(m_program, length, NULL, buffer.get());
    fprintf(stderr, "%s", buffer.get());
  }
}
