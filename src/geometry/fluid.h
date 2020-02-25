#ifndef FLUID_H
#define FLUID_H

#include <vector>
#include <algorithm>

#include "../common.h"
#include "../shader.hpp"

// template <typename T> 
// class Array3d
// {
//     size_t height, depth;
//     std::vector<T> data;

//   public:
//     Array3d(size_t width, size_t height, size_t depth) :
//     height(height),
//     depth(depth),
//     data(width * height * depth)
//     {}

//     T& at(size_t x, size_t y, size_t z)
//     { return data[x * height * depth + y * depth + z]; }

//     T at(size_t x, size_t y, size_t z) const
//     { return data[x * height * depth + y * depth + z]; }
// };

// template <typename T> 
// class Array2d
// {
//   public:
//     size_t height;
//     std::vector<T> data;
 
//     Array2d(size_t width, size_t height) :
//     height(height),
//     data(width * height)
//     {}

//     T& at(size_t i, size_t j)
//     { return data[(i)+(height+2)*(j)]; }

//     T at(size_t i, size_t j) const
//     { return data[(i)+(height+2)*(j)]; }

//     size_t size() const {
//         return data.size();
//     }

//     void fill(const T f) {
//        std::fill(data.begin(), data.end(), f);
//     }

//     void add_step(const Array2d &f, float dt {
//        std::fill(data.begin(), data.end(), f);
//     }
// };

class Fluid {
    mcl::Shader shader;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo[2]; // Vertexes and Colors
    std::vector<glm::vec3> points;
    std::vector<GLushort> indices;

    size_t _x_dim;
    size_t _y_dim;

    float * u, * v, * u_prev, * v_prev;
    float * dens, * dens_prev;

    // void compute_normals();
    void dens_step(float dt );
    void vel_step(float dt );
    void init_static_uniforms();
public:
    Fluid(size_t x_dim, size_t y_dim);

    void clear();

    void init();
    void update(float dt);
    void draw();
    void cleanup();
};

#endif // FLUID_H