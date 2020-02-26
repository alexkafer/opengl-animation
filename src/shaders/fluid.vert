#version 150 core

in vec3 in_position;
in float in_density;

out vec3 vposition;
out vec4 vcolor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vposition = vec3(model * vec4(in_position, 1.0));
    vcolor = vec4(0.1, 0.3, in_density > 0.7 ? 1 : in_density + 0.3, 1.0); //in_color;
	gl_Position = projection * view * model * vec4(in_position, 1.0);
    gl_PointSize = 2*in_density;
}

