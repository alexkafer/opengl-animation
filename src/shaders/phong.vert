#version 150 core

in vec3 in_position;
in vec3 in_normal;
in vec4 in_color;
in vec2 in_texture_coord;

out vec3 vposition;
out vec4 vcolor;
out vec3 vnormal;
out vec2 vtexture_coord;

uniform mat4 camera;
uniform mat4 model;

void main() {
    vtexture_coord = in_texture_coord;
    vnormal = in_normal;
    vcolor = in_color;
    vposition = in_position;
    
    // Apply all matrix transformations to vert
    gl_Position = camera * model * vec4(in_position, 1);
}

