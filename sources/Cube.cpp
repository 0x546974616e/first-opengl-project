#include <glad/glad.h> // OpenGL API
#include <GLFW/glfw3.h> // GLFW API
#include <stb_image.h> // STB Image

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <cstddef> // NULL
#include <iostream> // std::cerr, std::endl

#include "Cube.hpp"

#define STRINGIFY_HELPER(X) #X
#define STRINGIFY(X) STRINGIFY_HELPER(X)

#define POS(X, Y, Z) (X), (Y), (Z)
#define RGB(R, G, B) (R), (G), (B)
#define UV(U, V) (U), (V)

#define LOCATION0 0 // GLSL: layout (location = 0)
#define LOCATION1 1 // GLSL: layout (location = 1)
#define LOCATION2 2 // GLSL: layout (location = 2)

#define TEXTURE_UNIT0 0
#define TEXTURE_UNIT1 1
#define TEXTURE_UNIT2 2

static char const* CubeVertexShader = R"(
  #version 330 core
  layout (location = 0) in vec3 position;
  layout (location = 1) in vec3 color;
  layout (location = 2) in vec2 uv;
  out vec3 tr_Color;
  out vec2 tr_Texture;
  uniform mat4 model;
  uniform mat4 view;
  uniform mat4 proj;
  void main() {
    gl_Position = proj * view * model * vec4(position, 1.0f);
    tr_Color = color;
    tr_Texture = uv;
  }
)";

static char const* CubeFragmentShader = R"(
  #version 330 core
  in vec2 tr_Texture;
  in vec3 tr_Color;
  out vec4 tr_Fragment;
  uniform sampler2D texture1;
  uniform sampler2D texture2;
  void main() {
    tr_Fragment = mix(
      mix(
        texture(texture1, tr_Texture),
        texture(texture2, tr_Texture),
        0.2f
      ),
      vec4(tr_Color, 1.0f),
      0.2f
    );
  }
)";

#if TR_PLANE == 1
static GLfloat CubeVertices[] = {
  POS(-0.5f, +0.5f, 0.0f), RGB(1.0f, 0.0f, 0.0f), UV(0.0f, 1.0f), // Top-left
  POS(+0.5f, +0.5f, 0.0f), RGB(0.0f, 1.0f, 0.0f), UV(1.0f, 1.0f), // Top-right
  POS(+0.5f, -0.5f, 0.0f), RGB(0.0f, 0.0f, 1.0f), UV(1.0f, 0.0f), // Bottom-right
  POS(-0.5f, -0.5f, 0.0f), RGB(1.0f, 1.0f, 1.0f), UV(0.0f, 0.0f), // Bottom-left
};

static GLuint CubeIndices[] = {
  0, 1, 2, // First triangle
  2, 3, 0, // Second triangle
};
#else
static GLfloat CubeVertices[] = {
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(0.0f, 0.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.0f, 0.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(1.0f, 1.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(0.0f, 1.0f, 0.0f), UV(0.0f, 0.0f),

  POS(-0.5f, -0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(0.0f, 0.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.0f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(1.0f, 1.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.0f), UV(0.0f, 0.0f),

  POS(-0.5f, +0.5f, +0.5f), RGB(1.0f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(1.0f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 0.0f, 1.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(0.0f, 1.0f, 1.0f), UV(0.0f, 1.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(0.0f, 1.0f, 1.0f), UV(0.0f, 0.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(0.0f, 1.0f, 1.0f), UV(1.0f, 0.0f),

  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 0.5f, 0.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 0.0f), UV(1.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(1.0f, 0.5f, 0.0f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(0.5f, 1.0f, 0.5f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.5f, 1.0f, 0.5f), UV(0.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 0.5f), UV(1.0f, 0.0f),

  POS(-0.5f, -0.5f, -0.5f), RGB(0.5f, 0.0f, 1.0f), UV(0.0f, 1.0f),
  POS(+0.5f, -0.5f, -0.5f), RGB(0.5f, 0.0f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(0.5f, 0.0f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.1f), UV(1.0f, 0.0f),
  POS(-0.5f, -0.5f, +0.5f), RGB(1.0f, 1.0f, 0.1f), UV(0.0f, 0.0f),
  POS(-0.5f, -0.5f, -0.5f), RGB(1.0f, 1.0f, 0.1f), UV(0.0f, 1.0f),

  POS(-0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 1.0f), UV(0.0f, 1.0f),
  POS(+0.5f, +0.5f, -0.5f), RGB(1.0f, 0.5f, 1.0f), UV(1.0f, 1.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(1.0f, 0.5f, 1.0f), UV(1.0f, 0.0f),
  POS(+0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 1.0f), UV(1.0f, 0.0f),
  POS(-0.5f, +0.5f, +0.5f), RGB(0.5f, 1.0f, 1.0f), UV(0.0f, 0.0f),
  POS(-0.5f, +0.5f, -0.5f), RGB(0.5f, 1.0f, 1.0f), UV(0.0f, 1.0f),
};
#endif

GLuint CompileShader(GLenum type, char const* source) {
  GLuint shader = glCreateShader(type);
  glShaderSource(shader, 1, &source, NULL);
  glCompileShader(shader);

  int success;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  if (!success) {
    char const* name;
    char infoLog[512];
    switch (type) {
      case GL_VERTEX_SHADER: name = "GL_VERTEX_SHADER"; break;
      case GL_FRAGMENT_SHADER: name = "GL_FRAGMENT_SHADER"; break;
      default: name = "Unkown Type";
    }
    glGetShaderInfoLog(shader, sizeof(infoLog), NULL, infoLog);
    std::cerr << "ERROR::SHADER::" << name << "::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  return shader;
}

GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
  GLuint program = glCreateProgram();
  glAttachShader(program, vertexShader);
  glAttachShader(program, fragmentShader);
  glLinkProgram(program);

  int success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) { // check for linking errors
    char infoLog[512];
    glGetProgramInfoLog(program, sizeof(infoLog), NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  return program;
}

GLuint LoadTexture(char const* path) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  // Set the texture wrapping parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  // Set texture filtering parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  int width, height, channels;
  stbi_set_flip_vertically_on_load(true); // Flip Y-axis.
  unsigned char *data = stbi_load(path, &width, &height, &channels, 0);

  if (data != NULL) {
    if (channels == 3 || channels == 4) {
      GLint format = channels == 3 ? GL_RGB : GL_RGBA;
      glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, (GLenum) format, GL_UNSIGNED_BYTE, data);
      glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
      std::cerr << "Unsupported channel " << channels << " for \"" << path << '"' << std::endl;
    }
  }
  else {
    std::cerr << "Failed to load texture \"" << path << '"' << std::endl;
  }

  stbi_image_free(data);
  return texture;
}

Cube::Cube(void) {
  GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, CubeVertexShader);
  GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, CubeFragmentShader);
  m_shaderProgram = LinkProgram(vertexShader, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

#if TR_PLANE
  glGenBuffers(1, &m_EBO);
#endif

  glBindVertexArray(m_VAO);
  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);

#if TR_PLANE
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);
#endif

  glVertexAttribPointer(LOCATION0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (0 * sizeof(GLfloat)));
  glEnableVertexAttribArray(LOCATION0);
  glVertexAttribPointer(LOCATION1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (3 * sizeof(GLfloat)));
  glEnableVertexAttribArray(LOCATION1);
  glVertexAttribPointer(LOCATION2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (6 * sizeof(GLfloat)));
  glEnableVertexAttribArray(LOCATION2);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  m_texture1 = LoadTexture(TR_RESOURCES_DIR "/textures/container.jpg");
  m_texture2 = LoadTexture(TR_RESOURCES_DIR "/textures/awesomeface.png");
  glUseProgram(m_shaderProgram);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "texture1"), TEXTURE_UNIT0);
  glUniform1i(glGetUniformLocation(m_shaderProgram, "texture2"), TEXTURE_UNIT1);
}

Cube::~Cube(void) {
  glDeleteBuffers(1, &m_VBO);
#if TR_PLANE
  glDeleteBuffers(1, &m_EBO);
#endif
  glDeleteVertexArrays(1, &m_VAO);
  glDeleteProgram(m_shaderProgram);
  // glDeleteTextures(m_texture1);
  // glDeleteTextures(m_texture2);
}

void Cube::Render(GLFWwindow* window) {
  (void) m_texture1;
  (void) m_texture2;

  // Texture unit = texture location
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture1);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_texture2);

  glUseProgram(m_shaderProgram);
  glBindVertexArray(m_VAO);

  // glm::mat4 transform = glm::mat4(1.0f); // I
  // transform = glm::scale(transform, glm::vec3(0.5, 0.5, 0.5));
  // transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, 1.0f));
  // transform = glm::rotate(transform, (float) glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

  // GLint transformLoc = glGetUniformLocation(m_shaderProgram, "transform");
  // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

  // glm::mat4 model = glm::mat4(1.0f); // Identity
  glm::mat4 view  = glm::mat4(1.0f); // Identity
  glm::mat4 proj  = glm::mat4(1.0f); // Identity

  int width, height;
  glfwGetFramebufferSize(window, &width, &height);

  // model = glm::rotate(model, (float )glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
  view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
  proj  = glm::perspective(glm::radians(45.0f), (float) width / (float) height, 0.1f, 100.0f);

  // Retrieve the matrix uniform locations
  GLint modelLocation = glGetUniformLocation(m_shaderProgram, "model");
  GLint viewLocation  = glGetUniformLocation(m_shaderProgram, "view");
  GLint projLocation  = glGetUniformLocation(m_shaderProgram, "proj");

  // Pass them to the shaders (&view[0][0])
  glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(m_model));
  glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(projLocation, 1, GL_FALSE, glm::value_ptr(proj));

#if TR_PLANE
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
#else
  glDrawArrays(GL_TRIANGLES, 0, 36);
#endif

  glBindVertexArray(0);
}
