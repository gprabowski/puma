#version 460

out vec4 frag_color;

in float height;
in vec3 frag_norm;
in vec3 frag_pos;
in vec2 frag_tex;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform vec3 cam_pos;

uniform sampler2D height_map;
uniform sampler2D color;

void main() {
    vec4 color = vec4(texture(color, frag_tex).rgb, 1.0);
    vec3 ambient = vec3(0.1, 0.1, 0.1);

    vec3 normal = normalize( cross(dFdx(frag_pos), dFdy(frag_pos)) );

    vec3 light_dir = normalize(vec3(light_pos) - frag_pos);  
    float diff = max(dot(normal, light_dir), 0.0);
    vec3 diffuse = diff * vec3(light_color);

    vec3 view_dir = normalize(vec3(cam_pos) - frag_pos);
    vec3 ref_dir = reflect(-light_dir, normal);
    float spec = pow(max(dot(view_dir, ref_dir), 0.0), 32);
    vec3 specular = spec * vec3(light_color);

    frag_color = vec4((ambient + diffuse + specular) * vec3(color), color.a);
}
