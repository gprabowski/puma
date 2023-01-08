#version 460

out vec4 frag_color;
in vec3 normal;
in vec3 frag_pos;
in vec3 color;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 cam_pos;

void main() {
    vec3 ambient = vec3(0.2, 0.2, 0.2);
    float spec_pow = 0.5f;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(vec3(light_pos) - frag_pos);  
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * vec3(light_color);

    vec3 view_dir = normalize(vec3(cam_pos) - frag_pos);
    vec3 ref_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, ref_dir), 0.0), 32);
    vec3 specular = spec_pow * spec * vec3(light_color);

    frag_color = vec4((ambient + diffuse + specular) * color, 1.f);
}
