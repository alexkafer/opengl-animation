#ifndef SCENE_HPP
#define SCENE_HPP

#include "shader.hpp"
#include "geometry/Sphere.h"

void init_uniforms(mcl::Shader * shader);
void init_geometry(mcl::Shader * shader, GLuint * vbo, GLuint & vao);

void calc_animations();
void draw_scene();

#endif
