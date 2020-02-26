#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include <unordered_map>
#include "../common.h"
#include "../shader.hpp"

#include "../utils/stb_image.h"

struct PointMass {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 normal;
    glm::vec2 texture_coord;

    PointMass(const glm::vec3 &_position, const glm::vec2 &_texture_coord) 
        : position(_position), velocity(0.f), normal(0.f), texture_coord(_texture_coord) {} 
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

    int selected;

    std::vector<glm::vec3> forces;
    std::vector<PointMass> pointMasses;
    std::vector<std::pair<size_t, size_t>> edges;

    std::vector<GLushort> indices;

    spatial_hash_map spatial_hash;

    // std::vector<size_t> selected;

    GLuint vao;
    GLuint ibo; // Indicies
    GLuint vbo; // Vertexes
    GLuint texture_id;

    void update_forces(float dt);
    void check_collisions(float dt);
    void update_positions(float dt);

    void test_triangle_intersection(PointMass * triangle_points [3]);

    void compute_normals();

public:
    Cloth(size_t x, size_t y);

    void init(mcl::Shader & shader);

    void update(float dt);
    void draw(mcl::Shader & shader);
    void interaction(glm::vec3 origin, glm::vec3 direction);
    void drag_selected(glm::vec3 direction);

    void cleanup();

};

#endif // CLOTH_H