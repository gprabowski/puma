#version 460

layout(quads, fractional_odd_spacing, ccw) in;

uniform sampler2D height_map;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

in vec2 tese_tex[];

out float height;
out vec3 frag_norm;
out vec3 frag_pos;
out vec2 frag_tex;

void main() {
  float u = gl_TessCoord.x; 
  float v = gl_TessCoord.y;

  vec2 t00 = tese_tex[0];
  vec2 t01 = tese_tex[1];
  vec2 t10 = tese_tex[2];
  vec2 t11 = tese_tex[3];

  vec2 t0 = (t01 - t00) * u + t00;
  vec2 t1 = (t11 - t10) * u + t10;

  vec2 tex_coord = (t1 - t0) * v + t0;
  frag_tex = tex_coord;

  height = texture(height_map, tex_coord).r;

  vec4 p00 = gl_in[0].gl_Position;
  vec4 p01 = gl_in[1].gl_Position;
  vec4 p10 = gl_in[2].gl_Position;
  vec4 p11 = gl_in[3].gl_Position;

  vec4 uvec = p01 - p00; 
  vec4 vvec = p10 - p00;

  vec4 norm = normalize(vec4(cross(vvec.xyz, uvec.xyz), 0));

  vec4 p0 = (p01 - p00) * u + p00;
  vec4 p1 = (p11 - p10) * u + p10;

  vec4 p = (p1 - p0) * v + p0;

  p += norm * height;

  frag_pos = vec3(model * p);
  frag_norm = transpose(inverse(mat3(model))) * vec3(norm);

  gl_Position = proj * view * vec4(frag_pos, 1.0);
}
