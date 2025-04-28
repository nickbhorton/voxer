#version 450 core

in vec4 v_position;
in vec4 v_color;

out vec4 f_color;

uniform mat4 view;
uniform mat4 proj;

uniform vec3 offset;

void main() {
    gl_Position = proj * view * (v_position + vec4(offset, 0.0));
    f_color = v_color;
}
