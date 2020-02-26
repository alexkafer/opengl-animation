#ifndef FLUID_H
#define FLUID_H

#include <vector>

#include "../common.h"
#include "../utils/shader.h"

struct FluidPoint {
    glm::vec3 position;
    glm::vec3 normal;

    FluidPoint(const glm::vec3 &_position): position(_position), normal(0.f) {} ;
};

class Fluid {
    Shader shader;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo; // Vertexes
    std::vector<GLushort> indices;

    double t;
    size_t _x_dim;
    size_t _z_dim;
    size_t * vertex;

    std::vector<FluidPoint> points;
    std::vector<float> x_midpoint_heights;
    std::vector<float> z_midpoint_heights;
    std::vector<float> x_momentums;
    std::vector<float> z_momentums;
    std::vector<float> x_midpoint_momentums;
    std::vector<float> z_midpoint_momentums;

    void compute_normals();
    void init_static_uniforms();
public:
    Fluid(size_t x_dim, size_t z_dim);
    void init();
    void update(float dt);
    void draw();
    void cleanup();
};

#endif // FLUID_H