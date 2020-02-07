#version 150 core

in vec3 in_billboard;
in vec3 in_position;
in vec4 in_color;
in vec2 in_texture_coord;
in float in_size;

// out vec3 vposition;
out vec4 vcolor;
out vec2 vtexture_coord;

// Values that stay constant for the whole mesh.
uniform vec3 camera_right_worldspace;
uniform vec3 camera_up_worldspace;
uniform mat4 view_projection;

// uniform mat4 model;
// uniform mat4 view;
// uniform mat4 projection;

void main() {
    vec3 vertex_position = in_position
		+ camera_right_worldspace * in_billboard.x * in_size
		+ camera_up_worldspace * in_billboard.y * in_size;


    // vposition = vec3(model * vec4(in_position, 1.0));
    // vcolor = in_color;
    
	gl_Position = view_projection * vec4(vertex_position, 1.0);
    vtexture_coord = in_texture_coord;
    // vtexture_coord = in_billboard.xy + vec2(0.5, 0.5);
	vcolor = in_color;
    
    float dist = length(vertex_position);
    float att = inversesqrt(0.1f*dist);
    gl_PointSize = 8.0f * att;
}

