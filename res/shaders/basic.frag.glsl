#version 450 core

layout (location = 0) out vec4 fColor;

in vec4 f_color;

void main() {
    fColor = f_color;
}
