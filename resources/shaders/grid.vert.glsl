#version 330 core

#define GRID_NONE 0x0u
#define SHOW_GRID 0x1u

#define GRID_AXIS_X    0x010u
#define GRID_AXIS_Y    0x020u
#define GRID_AXIS_Z    0x040u
#define GRID_AXIS_MASK 0x070u

#define GRID_PLANE_XY   0x100u
#define GRID_PLANE_YZ   0x200u
#define GRID_PLANE_XZ   0x400u
#define GRID_PLANE_MASK 0x700u

uniform mat4 tr_view;
uniform mat4 tr_projection;
uniform vec3 tr_camera;
uniform float tr_far;
uniform uint tr_flags;
varying vec3 tr_position;

const int indices[6] = int[6](0, 2, 1, 2, 0, 3);
const vec2 vertices[4] = vec2[4](
  vec2(-1.0, -1.0), // Bottom Left
  vec2(+1.0, -1.0), // Bottom Right
  vec2(+1.0, +1.0), // Top Right
  vec2(-1.0, +1.0)  // Top Left
);

#define testFlag(value, flags) (((value) & (flags)) != 0u)

void main() {
  int index = indices[gl_VertexID];
  vec2 plane = vertices[index] * tr_far;

  // TODO: XZ should be tested first (more likely).
  if (testFlag(tr_flags, GRID_PLANE_XY)) {
    plane += tr_camera.xy;
    tr_position = vec3(plane.x, plane.y, 0.0);
  }
  else if (testFlag(tr_flags, GRID_PLANE_YZ)) {
    plane += tr_camera.yz;
    tr_position = vec3(0.0, plane.x, plane.y);
  }
  else { // testFlag(tr_flags, GRID_PLANE_XZ) [the default]
    plane += tr_camera.xz;
    tr_position = vec3(plane.x, 0.0, plane.y);
  }

  gl_Position = tr_projection * tr_view * vec4(tr_position, 1.0);
}
