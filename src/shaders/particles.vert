#version 150 core

in vec3 in_position;
in vec4 in_color;

out vec3 vposition;
out vec4 vcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vposition = vec3(model * vec4(in_position, 1.0));
    vcolor = in_color;
	gl_Position = projection * view * vec4(vposition, 1.0);
    gl_PointSize = 2.0;
}

