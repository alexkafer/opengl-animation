#version 150

in vec3 in_position;
in vec4 in_color;

out vec4 vcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vcolor = in_color;
    gl_PointSize = 10.0f;
    gl_Position = projection * view * model * vec4(in_position, 1.0);
}