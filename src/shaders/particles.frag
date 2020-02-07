#version 150 core

in vec4 vcolor;
in vec2 vtexture_coord;
out vec4 outColor;

uniform sampler2D sprite;

void main() {
    // outColor = texture(sprite, vtexture_coord);
    outColor = vcolor;
}