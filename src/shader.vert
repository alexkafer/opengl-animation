#version 150 core

in vec3 in_position;
in vec3 in_normal;

out vec3 vposition;
out vec3 vnormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal;

void main() {
    vposition = vec3(model * vec4(in_position, 1.0));
    vnormal = vec3(normal * vec4(in_normal, 1.0));
	gl_Position = projection * view * model * vec4(in_position, 1.0);
}

