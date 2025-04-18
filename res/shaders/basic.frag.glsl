#version 450 core

layout (location = 0) out vec4 fColor;

in vec4 f_color;
in vec2 f_uv;

void main() {
    float intensity = 0.75 + 0.25 * (length(f_uv) / sqrt(2));
    fColor = f_color * intensity;
}
