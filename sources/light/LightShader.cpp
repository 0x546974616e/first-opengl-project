#include "light/LightShader.hpp"

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <cmath>

TR_BEGIN_NAMESPACE()

#define TR_TOTAL 16

static char const* ShaderVersion = "#version 330 core\n";

static char const* IconPoint = "#define TR_STEP 1\n";
static char const* IconSun = "#define TR_STEP 2\n";

static char const* IconVertexShader = R"(
  uniform mat4 tr_view;
  uniform mat4 tr_projection;
  uniform float tr_aspectRatio;
  uniform vec3 tr_position;

  #define TR_TOTAL 16
  #define TR_RADIUS 0.05

  #define PI  3.1415926538f
  #define PI2 6.2831853071f

  void main(void) {
    // Get the screen-space position and compute the perspective division.
    gl_Position = tr_projection * tr_view * vec4(tr_position, 1.0);
    gl_Position /= gl_Position.w;

    bool firstPass = gl_VertexID < TR_TOTAL;
    // TODO: This can be done with a LUT (but KISS for development so far).
    int index = firstPass ? gl_VertexID : (gl_VertexID - TR_TOTAL) / TR_STEP;
    float angle = float(index) / float(TR_TOTAL);

    vec2 vertex;
    float ratio = (firstPass ? 2 : gl_VertexID % TR_STEP + 3) / 4.0;
    vertex.x = cos(angle * PI2) * TR_RADIUS * ratio;
    vertex.y = sin(angle * PI2) * TR_RADIUS * ratio;

    gl_Position.x += vertex.x * tr_aspectRatio;
    gl_Position.y += vertex.y;
  }
)";

static char const* RangeCircleVertexShader = R"(
  uniform mat4 tr_view;
  uniform mat4 tr_model;
  uniform mat4 tr_projection;
  uniform float tr_radii[2];

  // TODO: Extract it
  #define TR_TOTAL 32

  #define PI  3.1415926538f
  #define PI2 6.2831853071f

  void main(void) {
    vec2 vertex;
    int index = gl_VertexID % TR_TOTAL;
    float angle = float(index) / float(TR_TOTAL);
    float radius = tr_radii[gl_VertexID / TR_TOTAL];
    vertex.x = cos(angle * PI2) * radius;
    vertex.y = sin(angle * PI2) * radius;

    vec3 position = vec3(vertex.x, vertex.y, 0.0);
    gl_Position = tr_projection * tr_view * tr_model * vec4(position, 1.0);
  }
)";

static char const* FaceCircleVertexShader = R"(
  uniform mat4 tr_view;
  uniform mat4 tr_projection;
  uniform float tr_aspectRatio;
  uniform vec3 tr_position;
  uniform float tr_radius = 1.0;

  #define TR_TOTAL 32

  #define PI  3.1415926538f
  #define PI2 6.2831853071f

  void main(void) {
    vec2 vertex;
    int index = gl_VertexID % TR_TOTAL;
    float angle = float(index) / float(TR_TOTAL);
    vertex.x = cos(angle * PI2) * tr_radius;
    vertex.y = sin(angle * PI2) * tr_radius;

    gl_Position = tr_projection * tr_view * vec4(tr_position, 1.0);
    gl_Position.x += vertex.x * tr_aspectRatio;
    gl_Position.y += vertex.y;
  }
)";

static char const* LineVertexShader = R"(
  uniform mat4 tr_view;
  uniform mat4 tr_projection;
  uniform vec3 tr_positions[2];

  void main(void) {
    vec3 position = tr_positions[gl_VertexID % 2];
    gl_Position = tr_projection * tr_view * vec4(position, 1.0);
  }
)";

static char const* ConeLineVertexShader = R"(
  uniform vec4 tr_positions[2];

  void main(void) {
    gl_Position = tr_positions[gl_VertexID % 2];
  }
)";

static char const* FragmentShader = R"(
  #version 330 core
  out vec4 tr_fragment;
  #define L(X) (float(X) / 255.0)
  void main() { // #FFAF29 TODO Uniform
    tr_fragment = vec4(L(0xFFu), L(0xAFu), L(0x29u), 1.0);
  }
)";

static glm::mat4 CreateTransform(
  glm::vec3 const& position, glm::vec3 const& direction
) NOEXCEPT {
  // Normalize the direction vector.
  glm::vec3 forward = glm::normalize(direction);

  // Define an up vector (Y is up in OpenGL).
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Calculate the right vector.
  glm::vec3 right = glm::normalize(glm::cross(up, forward));

  // Recalculate the up vector based on the new right and forward vectors.
  up = glm::normalize(glm::cross(forward, right));

  // Create the transformation matrix (column-based).
  return glm::mat4(
    glm::vec4(right, 0.0f),
    glm::vec4(up, 0.0f),
    glm::vec4(forward, 0.0f),
    glm::vec4(position, 1.0f)
  );
}

static bool PlaneIntersection(
  glm::vec3 planePosition, glm::vec3 planeNormal,
  glm::vec3 rayPosition, glm::vec3 rayDirection,
  glm::vec3& result
) NOEXCEPT {
  // rayDirection = glm::normalize(rayDirection);
  // planeNormal = glm::normalize(planeNormal);
  float denumerator = glm::dot(rayDirection, planeNormal);
  if (denumerator >= -1e-6 && denumerator <= 1e-6) return false;
  float numerator = glm::dot(planePosition - rayPosition, planeNormal);
  float t = numerator / denumerator;
  result = rayPosition + rayDirection * t;
  return true;
}

LightShaderSun::LightShaderSun(glm::vec3 const& position) NOEXCEPT: m_position(position) {
  char const* vertex[] = { ShaderVersion, IconSun, IconVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderSun::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_projection", camera.Projection());
  m_shader.Bind("tr_aspectRatio", 1.0f / camera.AspectRatio());
  m_shader.Bind("tr_position", m_position);

  glDrawArrays(GL_LINES, 0, (GLsizei) TR_TOTAL * 3);
  glBindVertexArray(0);
}

LightShaderPoint::LightShaderPoint(glm::vec3 const& position) NOEXCEPT: m_position(position) {
  char const* vertex[] = { ShaderVersion, IconPoint, IconVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderPoint::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_projection", camera.Projection());
  m_shader.Bind("tr_aspectRatio", 1.0f / camera.AspectRatio());
  m_shader.Bind("tr_position", m_position);

  glDrawArrays(GL_LINES, 0, (GLsizei) TR_TOTAL * 2);
  glBindVertexArray(0);
}

LightShaderLine::LightShaderLine(
  glm::vec3 const& start,
  glm::vec3 const& end
) NOEXCEPT
  : m_start(start)
  , m_end(end)
{
  char const* vertex[] = { ShaderVersion, LineVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderLine::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_projection", camera.Projection());
  m_shader.Bind("tr_positions[0]", m_start);
  m_shader.Bind("tr_positions[1]", m_end);

  glDrawArrays(GL_LINES, 0, 2);
  glBindVertexArray(0);
}

LightShaderConeLine::LightShaderConeLine(
  glm::vec3 const& apex,
  glm::vec3 const& base,
  float radius
) NOEXCEPT
  : m_radius(radius)
  , m_apex(apex)
  , m_base(base)
{
  char const* vertex[] = { ShaderVersion, ConeLineVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderConeLine::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  glm::vec3 planePosition = m_base;
  glm::vec3 planeNormal = m_apex - planePosition;

  glm::vec3 rayPosition = camera.Position();
  glm::vec3 rayDirection = m_apex - rayPosition;

  glm::vec3 worldPoint;
  if (!PlaneIntersection(
    planePosition, planeNormal,
    rayPosition, rayDirection, worldPoint
  )) return;

  // TODO: Directly return the inverse.
  glm::mat4 model = CreateTransform(planePosition, planeNormal);
  glm::vec4 localPoint = glm::inverse(model) * glm::vec4(worldPoint, 1.0);
  localPoint /= localPoint.w; // Dehomogenisation

  float length = glm::length(glm::vec2(localPoint.x, localPoint.y));
  if (length <= m_radius + 1e-6) return;

  float tangentAngle = glm::acos(m_radius / length);
  float pointAngle = atan2f(localPoint.y, localPoint.x);

  // TODO: Define a macro for 32
  constexpr float TOTAL = 2.0f * glm::pi<float>() / 32.0f;

  float t1 = pointAngle + tangentAngle;
  float t2 = pointAngle - tangentAngle;

  t1 = glm::round(t1 / TOTAL) * TOTAL;
  t2 = glm::round(t2 / TOTAL) * TOTAL;

  glm::mat4 view = camera.LookAt();
  glm::mat4 projection = camera.Projection();

  glm::mat4 VP = projection * view;
  glm::mat4 MVP = VP * model;

  glm::vec4 apex = VP * glm::vec4(m_apex, 1.0);
  m_shader.Bind("tr_positions[0]", apex);

  glm::vec4 T1 = MVP * glm::vec4(m_radius * glm::cos(t1), m_radius * glm::sin(t1), 0.0, 1.0);
  glm::vec4 T2 = MVP * glm::vec4(m_radius * glm::cos(t2), m_radius * glm::sin(t2), 0.0, 1.0);

  m_shader.Bind("tr_positions[1]", T1);
  glDrawArrays(GL_LINES, 0, 2);

  m_shader.Bind("tr_positions[1]", T2);
  glDrawArrays(GL_LINES, 0, 2);

  glBindVertexArray(0);
}

LightShaderRangeCircle::LightShaderRangeCircle(
  glm::vec3 const& position,
  glm::vec3 const& direction,
  float innerRadius,
  float outerRadius
) NOEXCEPT
  : m_position(position)
  , m_direction(direction)
  , m_innerRadius(innerRadius)
  , m_outerRadius(outerRadius)
{
  char const* vertex[] = { ShaderVersion, RangeCircleVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderRangeCircle::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_model", CreateTransform(m_position, m_direction));
  m_shader.Bind("tr_projection", camera.Projection());

  m_shader.Bind("tr_radii[0]", m_innerRadius);
  m_shader.Bind("tr_radii[1]", m_outerRadius);

  glDrawArrays(GL_LINE_LOOP, 0, 32);
  glDrawArrays(GL_LINE_LOOP, 32, 32);
  glBindVertexArray(0);
}

LightShaderFaceCircle::LightShaderFaceCircle(
  glm::vec3 const& position,
  float radius
) NOEXCEPT
  : m_radius(radius)
  , m_position(position)
{
  char const* vertex[] = { ShaderVersion, FaceCircleVertexShader };
  m_shader.Attach(GL_VERTEX_SHADER, TR_ARRAYSIZE(vertex), vertex);
  m_shader.Attach(GL_FRAGMENT_SHADER, FragmentShader);
  m_shader.Link();

  glGenVertexArrays(1, &m_VAO);
}

void LightShaderFaceCircle::Render(Camera const& camera) NOEXCEPT {
  m_shader.Use();
  glBindVertexArray(m_VAO);

  m_shader.Bind("tr_view", camera.LookAt());
  m_shader.Bind("tr_projection", camera.Projection());
  m_shader.Bind("tr_aspectRatio", 1.0f / camera.AspectRatio());
  m_shader.Bind("tr_position", m_position);
  m_shader.Bind("tr_radius", m_radius);

  glDrawArrays(GL_LINE_LOOP, 0, 32);
  glBindVertexArray(0);
}

TR_END_NAMESPACE()
