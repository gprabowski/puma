#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 color;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

float gridSize = 10000.0f;
float gridCellSize = 0.05f;

vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
vec4 gridColorThick = vec4(0.0, 0.0, 0.0, 1.0);

const float gridMinPixelsBetweenCells = 2.0;

out vec2 uv;

void main() {
    vec3 vpos = vec3(pos) * gridSize;
    gl_Position = proj * view * vec4(vpos, 1.0);
    uv = vpos.xz;
}
