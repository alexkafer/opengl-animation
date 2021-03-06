#ifndef FLUID_H
#define FLUID_H

#include <vector>
#include <algorithm>

#include "../common.h"
#include "../utils/shader.h"

class Fluid {
    Shader shader;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo[2]; // Vertexes and Colors
    glm::vec3 * points;
    std::vector<GLushort> indices;


    size_t _total_size;
    size_t _x_dim;
    size_t _y_dim;
    size_t _z_dim;

    int addforce[3];
    int addsource;

    float * x_vel, * x_vel_prev;
    float * y_vel, * y_vel_prev;
    float * z_vel, * z_vel_prev;
    float * dens, * dens_prev;

    void init_static_uniforms();
    void update_force_source(float * d, float * u, float * v, float * w );
public:
    Fluid(size_t x_dim, size_t y_dim, size_t z_dim);

    void clear();

    void interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down);
    void key_down(int key);
    void init();
    void update(float dt);
    void draw();
    void cleanup();
};

#endif // FLUID_H