// identity vertex shader
#version 150
in vec4 vertex_position;

void main()  {
	gl_Position = vertex_position;  // pass on the vertex position unchanged
}
