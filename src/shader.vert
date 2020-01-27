#version 150 core

in vec3 in_position;
in vec3 in_color;
in vec3 in_normal;

out vec3 vposition;
out vec3 vcolor;
out vec3 vnormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vcolor = in_color;

    vec4 pos = projection * view * model * vec4(in_position, 1.0);
	vnormal = in_normal;
	vposition = vec3( pos );

	gl_Position = pos;
}