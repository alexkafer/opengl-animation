#version 150 core

in vec3 in_position;
in vec4 in_color;
in float in_age;

out vec3 vposition;
out vec4 vcolor;
out vec2 vtexture_coord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    vposition = vec3(model * vec4(in_position, 1.0));
    vcolor = in_color;
    vtexture_coord = vec2((int(in_age * 64.f) % 8) / 8.0, (int(in_age * 64.f) / 8) / 16.0);
    
	gl_Position = projection * view * vec4(vposition, 1.0);
    
    float dist = length(vposition.xyz);
    float att = inversesqrt(0.1f*dist);
    gl_PointSize = 8.0f * att;
}

