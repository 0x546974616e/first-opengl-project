#ifndef TR_CUBE_HPP
#define TR_CUBE_HPP

#include <glad/glad.h> // OpenGL API
#include <GLFW/glfw3.h> // GLFW API
#include <glm/mat4x4.hpp>

#define TR_PLANE 0

class Cube final {
private:
  GLuint m_VAO; // Vertex Array Object
  GLuint m_VBO; // Vertex Buffer Object
  GLuint m_texture1;
  GLuint m_texture2;
  GLuint m_shaderProgram;

#if TR_PLANE
  GLuint m_EBO; // Element Buffer Object;
#endif

  glm::mat4 m_model{1.0f};

public:
  Cube(void); ~Cube(void);
  void Render(GLFWwindow* window);
  void setModel(glm::mat4 const& model) {
    m_model = model;
  }
};

#endif // TR_CUBE_HPP
