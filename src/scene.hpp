#ifndef SCENE_HPP
#define SCENE_HPP

#include "shader.hpp"
#include "geometry/Sphere.h"

void init_static_uniforms(mcl::Shader * shader);
void init_geometry(mcl::Shader * shader, GLuint & vbo, GLuint & ibo, GLuint & vao);

void draw_scene(mcl::Shader * shader, GLuint & vbo, GLuint & ibo);

#endif
