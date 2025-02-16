#include <glad/glad.h> // OpenGL API

#include <fstream> // std::ifstream{}
#include <memory> // std::unique_ptr{}
#include <sstream> // std::stringstream{}
#include <string> // std::string{}

#include "helper.hpp" // NOEXCEPT
#include "Shader.hpp" // Self{}
#include "Log.hpp" // TR_ERROR()

static char const* ShaderType(GLenum type) NOEXCEPT {
  switch (type) {
    case GL_COMPUTE_SHADER:
      return "GL_COMPUTE_SHADER";
    case GL_FRAGMENT_SHADER:
      return "GL_FRAGMENT_SHADER";
    case GL_GEOMETRY_SHADER:
      return "GL_GEOMETRY_SHADER";
    case GL_VERTEX_SHADER:
      return "GL_VERTEX_SHADER";
    default:
      return "??";
  }
}

void Shader::Attach(GLenum type, std::string_view source) NOEXCEPT {
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
    TR_ERROR("Shader::Compile(%s) Error:\n%*.*s",
      ShaderType(type), length, length, buffer.get()
    );
  }

  // https://gamedev.stackexchange.com/questions/47910/after-a-succesful-gllinkprogram-should-i-delete-detach-my-shaders
  glAttachShader(m_program, shader);
  glDeleteShader(shader);
}

void Shader::Attach(std::string_view filename) NOEXCEPT {
  GLenum type = 0;
  std::string_view search = filename;

  do {
    size_t index = search.rfind(".");
    if (index == std::string_view::npos) {
      TR_ERROR("Shader extension is missing: %s", filename.data());
      return;
    }

    std::string_view extension = search.substr(index + 1);
         if (extension == "comp") type = GL_COMPUTE_SHADER;
    else if (extension == "frag") type = GL_FRAGMENT_SHADER;
    else if (extension == "geom") type = GL_GEOMETRY_SHADER;
    else if (extension == "vert") type = GL_VERTEX_SHADER;
    else if (extension == "glsl") search = search.substr(0, index);
    else {
      TR_ERROR("Unknown shader extension: %s", filename.data());
      return;
    }
  } while(type == 0);

  std::string path = TR_RESOURCES_DIR "/shaders/";
  std::ifstream file(path += filename);
  std::stringstream buffer;
  buffer << file.rdbuf();
  std::string source = buffer.str();
  Attach(type, source);
}

void Shader::Link(void) NOEXCEPT {
  GLint length;
  glLinkProgram(m_program);

  glGetProgramiv(m_program, GL_LINK_STATUS, &m_status);
  if(m_status == GL_FALSE) {
    glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &length);
    std::unique_ptr<char[]> buffer(new char[static_cast<size_t>(length)]);
    glGetProgramInfoLog(m_program, length, NULL, buffer.get());
    TR_ERROR("Shader::Link Error:\n%*.*s", length, length, buffer.get());
  }
}
