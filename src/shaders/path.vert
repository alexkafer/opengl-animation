#version 150

in vec3 in_position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_PointSize = 10.0f;
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}