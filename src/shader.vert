#version 150 core

in vec2 position;
in vec3 inColor;
out vec3 Color;

void main() {
    Color = inColor;
    gl_Position = vec4(position, 0.0, 1.0);
}