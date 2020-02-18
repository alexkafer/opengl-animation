#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include <unordered_map>
#include "../common.h"
#include "../shader.hpp"

struct PointMass {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 normal;

    PointMass(const glm::vec3 &_position) 
        : position(_position), velocity(0.f), normal(0.f) {} 
};

// This spatial map stores a position with a list of points in that region
typedef std::unordered_multimap<
        size_t, // The hashed position
        size_t//,  // The point index
        // std::function<size_t(glm::vec3)> // The hasher
    > spatial_hash_map;

class Cloth {
    size_t _x_dim;
    size_t _y_dim;

    std::vector<glm::vec3> forces;
    std::vector<PointMass> pointMasses;
    std::vector<std::pair<size_t, size_t>> edges;

    std::vector<GLushort> indices;

    spatial_hash_map spatial_hash;

    std::vector<size_t> selected;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo; // Vertexes

    void update_forces(float dt);
    void check_collisions(float dt);
    void update_positions(float dt);

    void compute_normals();

public:
    Cloth(size_t x, size_t y);

    void init(mcl::Shader & shader);

    void update(float dt);
    void draw(mcl::Shader & shader);
    int find_object(glm::vec3 origin, glm::vec3 direction);
    void drag_object(int object, glm::vec3 direction);

    void cleanup();

};

#endif // CLOTH_H