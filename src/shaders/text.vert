#version 330 core
in vec4 in_vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(in_vertex.xy, 0.0, 1.0);
    TexCoords = in_vertex.zw;
} 