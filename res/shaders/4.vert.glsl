#version 450 core

in uint v_position;

out vec4 f_color;

uniform mat4 view;
uniform mat4 proj;

uniform vec3 offset;

uniform vec4 colors[256];

void main() {
    vec4 v_position_f = vec4(
        float((v_position >> 0 ) & 255),
        float((v_position >> 8 ) & 255),
        float((v_position >> 16) & 255),
        1.0
    );
    uint ci = (v_position >> 24) & 255;
    gl_Position = proj * view * (v_position_f + vec4(offset, 0.0));
    f_color = colors[ci];
}
