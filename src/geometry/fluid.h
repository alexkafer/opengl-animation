#ifndef FLUID_H
#define FLUID_H

#include <vector>
#include <algorithm>

#include "../common.h"
#include "../shader.hpp"

class Fluid {
    mcl::Shader shader;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo[2]; // Vertexes and Colors
    glm::vec3 * points;
    std::vector<GLushort> indices;

    size_t _x_dim;
    size_t _z_dim;
    size_t _size;

    int source;

    float * u, * v, * u_prev, * v_prev;
    float * dens, * dens_prev;

    void init_static_uniforms();
public:
    Fluid(size_t x_dim, size_t y_dim);

    void clear();

    int find_object(glm::vec3 origin, glm::vec3 direction);
    void drag_object(int object, glm::vec3 direction);

    void init();
    void update(float dt);
    void draw();
    void cleanup();
};

#endif // FLUID_H