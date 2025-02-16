#version 330 core

// Highly inspired from Blender (GOLD <3):
// https://github.com/blender/blender/blob/main/source/blender/draw/engines/overlay/shaders/overlay_grid_vert.glsl
// https://github.com/blender/blender/blob/main/source/blender/draw/engines/overlay/shaders/overlay_grid_frag.glsl
// https://developer.nvidia.com/gpugems/gpugems2/part-iii-high-quality-rendering/chapter-22-fast-prefiltered-lines
// https://fr.wikipedia.org/wiki/Segment_circulaire

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

#define GRID_STEPS 8
const float gridSteps[GRID_STEPS] = float[GRID_STEPS](
  0.001f, 0.01f, 0.1f, 1.0f, 10.0f, 100.0f, 1000.0f, 10000.0f
);

in vec3 tr_position;
out vec4 tr_fragment;

uniform mat4 tr_view;
uniform uint tr_flags;
uniform vec3 tr_camera;

uniform float tr_near, tr_far;
uniform float tr_lineSize = 1.0; // TODO: DPI

uniform vec4 tr_colorGrid;
uniform vec4 tr_colorGridEmphasis;
uniform vec4 tr_colorGridAxisX = vec4(1.0, 0.0, 0.0, 1.0);
uniform vec4 tr_colorGridAxisY = vec4(0.0, 1.0, 0.0, 1.0);
uniform vec4 tr_colorGridAxisZ = vec4(0.0, 0.0, 1.0, 1.0);

#define M_1_SQRTPI 0.5641895835477563 // 1 / sqrt(pi)
#define DISC_RADIUS (M_1_SQRTPI * 1.05)
#define GRID_LINE_SMOOTH_START (0.5 + DISC_RADIUS)
#define GRID_LINE_SMOOTH_END (0.5 - DISC_RADIUS)

///
/// Anti-aliased grid lines.
///
/// The goal is to determine how much of a pixel is covered by a grid line.
///
/// Instead of calculating the exact area of intersection between a line and a
/// square pixel, the pixel is approximated as a circle with the same area. The
/// intersection of the line with the circle creates a circular segment.
///
/// Calculating the exact area of a circular segment is computationally
/// expensive, so the `smoothstep()` function is used as an approximation.
///
/// The 1.05 factor slightly increases the circle's radius for better visual
/// results.
///
#define GRID_LINE_STEP(DISTANCE) \
  smoothstep(GRID_LINE_SMOOTH_START, GRID_LINE_SMOOTH_END, DISTANCE)

///
/// Calculate the grid line intensity at a given coordinate.
///
/// @param position A 2D vector corresponding to the world coordinates of the grid.
/// @param fwidthPosition A 2D vector representing the absolute rate of change of the coordinates.
///
float getGrid(vec2 position, vec2 fwidthPosition, float gridScale) {
  float halfSize = gridScale / 2.0;

  // Triangular wave pattern, amplitude is [0, half_size].
  vec2 gridDomain = abs(mod(position + halfSize, gridScale) - halfSize);

  // Modulate by the absolute rate of change of the coordinates
  // (make line have the same width under perspective).
  gridDomain /= fwidthPosition;

  // Collapse waves.
  float lineDistance = min(gridDomain.x, gridDomain.y);

  // Circular segment area.
  return GRID_LINE_STEP(lineDistance - tr_lineSize);
}

///
/// This function calculates the intensity of the X, Y, and Z axes lines.
///
/// @param position The 3D coordinates representing the distance from each axis plane (XY, YZ, XZ).
/// @param fwidthPosition Perspective correction for each axis.
///
vec3 getAxes(vec3 position, vec3 fwidthPosition, float lineSize) {
  vec3 axesDomain = abs(position);

  // Modulate by the absolute rate of change of the coordinates
  // (make line have the same width under perspective).
  axesDomain /= fwidthPosition;

  // Circular segment area.
  return GRID_LINE_STEP(axesDomain - (lineSize + tr_lineSize));
}

///
/// Test if a flag (or a mask) validates the given value.
///
/// (To test an exact match replace `!= 0u` with `== (FLAGS)`).
///
#define testFlag(VALUE, FLAGS) (((VALUE) & (FLAGS)) != 0u)

///
/// Linear step in [0.0, 1.0] of VALUE from MIN to MAX.
///
#define linearstep(MIN, MAX, VALUE) \
  (clamp(((VALUE) - (MIN)) / abs((MAX) - (MIN)), 0.0, 1.0))

///
/// Draw anti-aliased grid and axes of different sizes with smooth blending
/// between levels of detail.
///
void main(void) {
  // A vector representing the rate of change of the coordinates with respect
  // to screen space (used for perspective correction).
  vec3 ddxPosition = dFdx(tr_position), ddyPosition = dFdy(tr_position);
  vec3 fwidthPosition = abs(ddxPosition) + abs(ddyPosition);

  vec3 localPosition = tr_camera - tr_position;
  float localDistance = length(localPosition);
  localPosition /= localDistance;

  float angle;
  // TODO: XZ should be tested first (more likely).
  if (testFlag(tr_flags, GRID_PLANE_XY)) {
    angle = localPosition.z;
  }
  else if (testFlag(tr_flags, GRID_PLANE_YZ)) {
    angle = localPosition.x;
  }
  else { // testFlag(tr_flags, GRID_PLANE_XZ) [the default]
    angle = localPosition.y;
  }

  // Fades the grid as the camera becomes more parallel to it.
  angle = 1.0 - abs(angle); angle *= angle;
  float fade = 1.0 - angle * angle;

  // Fade the grid close to the near and far view plane.
  fade *= smoothstep(0.0, tr_near * 10, localDistance - tr_near);
  fade *= 1.0 - smoothstep(0.0, tr_far / 2, localDistance - tr_far / 2);

  if (testFlag(tr_flags, SHOW_GRID) && testFlag(tr_flags, GRID_PLANE_MASK)) {
    // Using `dot(dFdxPos, inverse(tr_view)[0].xyz)` would have been enough.
    float resolution = max(
      // TODO: UBO with view and inverse view matrixes.
      dot(ddxPosition, inverse(tr_view)[0].xyz),
      dot(ddyPosition, inverse(tr_view)[1].xyz)
    );

    // The grid begins to appear when it comprises 4 pixels.
    resolution *= 4;

    int stepIndex = GRID_STEPS - 1;
    // Loop backwards a compile-time-constant number of steps.
    for (int index = GRID_STEPS - 2; index >= 0; --index) {
      stepIndex = (resolution < gridSteps[index]) ? index : stepIndex; // Branchless.
    }

    // From biggest to smallest.
    float scale0 = stepIndex > 0 ? gridSteps[stepIndex - 1] : 0.0;
    float scaleA = gridSteps[stepIndex];
    float scaleB = gridSteps[min(stepIndex + 1, GRID_STEPS - 1)];
    float scaleC = gridSteps[min(stepIndex + 2, GRID_STEPS - 1)];

    float blend = linearstep(scale0 + scale0, scaleA + scaleA, resolution + resolution);
    // Subtract from 1.0 to fix blending when `scale0 == scaleA`.
    blend = 1.0 - blend; blend = blend * blend * blend;

    vec2 gridPosition, fwidthGrid;
    // TODO: XZ should be tested first (more likely).
    if (testFlag(tr_flags, GRID_PLANE_XY)) {
      fwidthGrid = fwidthPosition.xy;
      gridPosition = tr_position.xy;
    }
    else if (testFlag(tr_flags, GRID_PLANE_YZ)) {
      fwidthGrid = fwidthPosition.yz;
      gridPosition = tr_position.yz;
    }
    else { // testFlag(tr_flags, GRID_PLANE_XZ) [the default]
      fwidthGrid = fwidthPosition.xz;
      gridPosition = tr_position.xz;
    }

    float gridA = getGrid(gridPosition, fwidthGrid, scaleA);
    float gridB = getGrid(gridPosition, fwidthGrid, scaleB);
    float gridC = getGrid(gridPosition, fwidthGrid, scaleC);

    tr_fragment = tr_colorGrid;
    tr_fragment.a *= gridA * blend;
    tr_fragment = mix(tr_fragment, mix(tr_colorGrid, tr_colorGridEmphasis, blend), gridB);
    tr_fragment = mix(tr_fragment, tr_colorGridEmphasis, gridC);
  }
  else {
    tr_fragment = vec4(tr_colorGrid.rgb, 0.0);
  }

  // TODO: Artefacts appear when axes are far.
  if (testFlag(tr_flags, GRID_AXIS_MASK)) {
    vec3 axes = getAxes(tr_position, fwidthPosition, 0.1);

    if (testFlag(tr_flags, GRID_AXIS_X)) {
      tr_fragment.a = max(tr_fragment.a, axes.x);
      tr_fragment.rgb = mix(tr_fragment.rgb, tr_colorGridAxisX.rgb, axes.x);
    }
    if (testFlag(tr_flags, GRID_AXIS_Y)) {
      tr_fragment.a = max(tr_fragment.a, axes.y);
      tr_fragment.rgb = mix(tr_fragment.rgb, tr_colorGridAxisY.rgb, axes.y);
    }
    if (testFlag(tr_flags, GRID_AXIS_Z)) {
      tr_fragment.a = max(tr_fragment.a, axes.z);
      tr_fragment.rgb = mix(tr_fragment.rgb, tr_colorGridAxisZ.rgb, axes.z);
    }
  }

  // tr_fragment = vec4(tr_position, 1.0);
  tr_fragment.a *= fade;
}
