#version 460 

layout (vertices=4) out;

in vec2 tesc_tex[];

out vec2 tese_tex[];

void main()
{
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  tese_tex[gl_InvocationID] = tesc_tex[gl_InvocationID];

  gl_TessLevelOuter[0] = 64;
  gl_TessLevelOuter[1] = 64;
  gl_TessLevelOuter[2] = 64;
  gl_TessLevelOuter[3] = 64;

  gl_TessLevelInner[0] = 64;
  gl_TessLevelInner[1] = 64;
}
