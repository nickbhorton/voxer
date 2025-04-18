#version 450 core

in vec4 v_position;
in vec4 v_color;
in vec2 v_uv;

out vec4 f_color;
out vec2 f_uv;

uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * v_position;
    f_color = v_color;
    f_uv = v_uv;
}
