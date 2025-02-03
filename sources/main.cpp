#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <cstdlib> // EXIT_
#include <cmath> // sin()

// settings
static unsigned int const SCR_WIDTH = 800;
static unsigned int const SCR_HEIGHT = 600;

// Shaders are little programs that rest on the GPU.
// In a basic sense, shaders are nothing more than programs transforming inputs to outputs.
// Shaders are also very isolated programs in that they're not allowed to communicate with each other.
static char const* vertexShaderSource = R"(
  #version 330 core
  layout (location = 0) in vec3 aPos;
  void main() {
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
  }
)";

static char const* fragmentShaderSource = R"(
  #version 330 core
  out vec4 FragColor;
  uniform vec4 ourColor;
  void main() {
    FragColor = ourColor + vec4(1.0f, 0.0f, 0.0f, 0.0f);
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
  }
)";

// GLFW: whenever the window size changed (by OS or user resize) this callback function executes
static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // Make sure the viewport matches the new window dimensions; note that width and
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

static void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

// Yes. glfwGetKey() is meant to be used for continuous key input. GLFW manual
// lists glfwSetKeyCallback as a better alternative if you want one-time
// notification about key press.
// <https://www.glfw.org/docs/latest/input_guide.html#input_key>
// <https://stackoverflow.com/questions/52492426/glfw-switching-boolean-toggle>
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
  if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
    GLint polygonMode[2];
    glGetIntegerv(GL_POLYGON_MODE, polygonMode);

    GLenum newPolygonMode = GL_FILL;
    switch (polygonMode[1]) {
      case GL_FILL: newPolygonMode = GL_LINE; break;
      case GL_LINE: newPolygonMode = GL_POINT; break;
      case GL_POINT: newPolygonMode = GL_FILL; break;
    }

    // GL_FRONT_AND_BACK: apply it to the front and back of all triangles.
    glPolygonMode(GL_FRONT_AND_BACK, newPolygonMode);
  }
}

int main(void) {
  // GLFW: initialize and configure
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // GLFW window creation
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetKeyCallback(window, keyCallback);

  // GLAD: load all OpenGL function pointers
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    glfwTerminate();
    return EXIT_FAILURE;
  }

  int success;
  char infoLog[512];

  // Vertex shader
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, sizeof(infoLog), NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Fragment shader
  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, sizeof(infoLog), NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
  }

  // Link shaders
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) { // check for linking errors
    glGetProgramInfoLog(shaderProgram, sizeof(infoLog), NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
  }

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Normalized Device Coordinates (NDC)
  // Tightly packed
  float vertices[] = {
     0.5f,  0.5f, 0.0f, // Top right
     0.5f, -0.5f, 0.0f, // Bottom right
    -0.5f, -0.5f, 0.0f, // Bottom left
    -0.5f,  0.5f, 0.0f, // Top left
  };

  unsigned int indices[] = {
    0, 1, 3, // First Triangle
    1, 2, 3, // Second Triangle
  };

  // Vertex Array Object
  GLuint VAO;
  glGenVertexArrays(1, &VAO);

  // Vertex Buffer Object
  GLuint VBO;
  glGenBuffers(1, &VBO);

  // Element Buffer Object;
  GLuint EBO;
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO); // Now the VAO.

  // - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
  // - GL_STATIC_DRAW: the data is set only once and used many times.
  // - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


  // The last element buffer object that gets bound while a VAO is bound, is
  // stored as the VAO’s element buffer object. Binding to a VAO then also
  // automatically binds that EBO.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // [...] which VBO it takes its data from is determined by the VBO currently
  // bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer().
  #define VERTEX_ATTRIBUTE_LOCATION 0 // GLSL: layout (location = 0)
  glVertexAttribPointer(VERTEX_ATTRIBUTE_LOCATION, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
  glEnableVertexAttribArray(VERTEX_ATTRIBUTE_LOCATION);

  // Note that this is allowed, the call to glVertexAttribPointer registered VBO
  // as the vertex attribute's bound vertex buffer object so afterwards we can
  // safely unbind.
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // A VAO stores the glBindBuffer() calls when the target is
  // GL_ELEMENT_ARRAY_BUFFER. This also means it stores its unbind calls so make
  // sure you don't unbind the element array buffer before unbinding your VAO,
  // otherwise it doesn't have an EBO configured.

  // You can unbind the VAO afterwards so other VAO calls won't accidentally
  // modify this VAO, but this rarely happens. Modifying other VAOs requires a
  // call to glBindVertexArray anyways so we generally don't unbind VAOs (nor
  // VBOs) when it's not directly necessary.
  glBindVertexArray(0);

  // Note that finding the uniform location does not require you to use the
  // shader program first, but updating a uniform does require you to first use
  // the program.  If it returns -1, it could not find the location.
  GLint vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");

  // render loop
  while (!glfwWindowShouldClose(window)) {
    // Input
    processInput(window);

    // Render
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f); // state-setting
    glClear(GL_COLOR_BUFFER_BIT); // state-using

    {
      // Draw triangle.
      glUseProgram(shaderProgram);
      glBindVertexArray(VAO);

      {
        // update shader uniform
        double timeValue = glfwGetTime();
        GLfloat greenValue = static_cast<GLfloat>(sin(timeValue) / 2.0 + 0.5);
        glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);
      }

      // glDrawArrays(GL_TRIANGLES, 0, 3);
      glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0); // No need to unbind it every time
    }

    // GLFW: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  GLint nrAttributes;
  glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
  // OpenGL guarantees 16 at least 16 4-component vertex attributes.
  std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteProgram(shaderProgram);

  // Clearing all previously allocated GLFW resources.
  glfwDestroyWindow(window);
  glfwTerminate();

  return EXIT_SUCCESS;
}
