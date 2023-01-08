#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec3 col;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 frag_pos;
out vec3 normal;
out vec3 color;

void main() {
    gl_Position = proj * view * model * vec4(pos, 1.0);
    frag_pos = vec3(model * vec4(pos, 1.0));
    normal = transpose(inverse(mat3(model))) * norm;
    color = col;
}
