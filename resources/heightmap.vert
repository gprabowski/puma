#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;

uniform sampler2D height_map;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 tesc_tex;

void main() {
  gl_Position = vec4(pos, 1.0);

  tesc_tex = tex;
}
