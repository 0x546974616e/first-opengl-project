#ifndef TR_SHADER_HPP
#define TR_SHADER_HPP

#include <glad/glad.h> // OpenGL API
#include <glm/mat4x4.hpp> // glm::mat4{}
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr()

#include <cstdio> // fprintf(), stderr
#include <string_view> // std::string_view{}

#include "helper.hpp" // TR_ERROR()

class Shader final {
public:
  constexpr Shader() noexcept {
    m_program = glCreateProgram();
  }

  constexpr ~Shader() noexcept {
    glDeleteProgram(m_program);
  }

  constexpr void Use(void) noexcept {
    glUseProgram(m_program);
  }

  constexpr void Bind(GLint location, GLint value) noexcept {
    glUniform1i(location, value);
  }

  constexpr void Bind(GLint location, GLfloat value) noexcept {
    glUniform1f(location, value);
  }

  constexpr void Bind(GLint location, glm::mat4 const& matrix) noexcept {
    // OpenGL and GLM matrixes are column-major ordered. GL_FALSE = No transpose.
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
  }

  template <typename Value>
  constexpr void Bind(std::string_view name, Value&& value) noexcept {
    GLint location = glGetUniformLocation(m_program, name.data());
    if (location == -1) TR_ERROR("Missing Uniform: %s", name.data());
    else Bind(location, std::forward<Value>(value));
  }

  constexpr GLuint Get() const noexcept {
    return m_program;
  }

  /// Load from "resources/shaders/".
  void Attach(std::string_view filename) noexcept;
  void Attach(GLenum type, std::string_view source) noexcept;

  void Link(void) noexcept;

private:
  // TODO: Is there a way to increment an internal counter and retrieve it? (Like OpenCL)
  TR_DELETE_COPY_CTOR(Shader);
  TR_DELETE_MOVE_CTOR(Shader);

  GLuint m_program;
};

#endif // TR_SHADER_HPP
