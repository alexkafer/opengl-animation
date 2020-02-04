#version 150 core

in vec3 in_position;
in vec3 in_normal;
in vec3 in_color;
// in vec3 in_texture;

out vec3 vposition;
out vec3 vnormal;
out vec3 vcolor;
// out vec3 vtexture;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normal;

void main() {
    vposition = vec3(model * vec4(in_position, 1.0));
    vnormal = vec3(normal * vec4(in_normal, 1.0));
    vcolor = in_color;
    // vtexture = in_texture;
	gl_Position = projection * view * model * vec4(in_position, 1.0);
}

