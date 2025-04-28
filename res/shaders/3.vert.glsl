#version 450 core

in uint v_position;
in uint v_color;

out vec4 f_color;

uniform mat4 view;
uniform mat4 proj;

uniform vec3 offset;

void main() {
    vec4 v_position_f = vec4(
        float((v_position >> 0 ) & 255),
        float((v_position >> 8 ) & 255),
        float((v_position >> 16) & 255),
        1.0
    );
    vec4 v_color_f = vec4(
        float((v_color >> 0 ) & 255) / 255.0,
        float((v_color >> 8 ) & 255) / 255.0,
        float((v_color >> 16) & 255) / 255.0,
        1.0
    );

    gl_Position = proj * view * (v_position_f + vec4(offset, 0.0));
    f_color = v_color_f;
}
