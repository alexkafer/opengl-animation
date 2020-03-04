#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include <unordered_map>
#include "../common.h"
#include "../utils/shader.h"

#include "../utils/stb_image.h"
#include "../entities/entity.h"

struct PointMass {
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 normal;
    glm::vec2 texture_coord;

    PointMass(): position(0.f), velocity(0.f), normal(0.f), texture_coord(0.f) {}

    PointMass(const glm::vec3 &_position, const glm::vec2 &_texture_coord) 
        : position(_position), velocity(0.f), normal(0.f), texture_coord(_texture_coord) {} 
};

// This spatial map stores a position with a list of points in that region
typedef std::unordered_multimap<
        size_t, // The hashed position
        size_t//,  // The point index
        // std::function<size_t(glm::vec3)> // The hasher
    > spatial_hash_map;

class Cloth: public Entity {
    size_t _x_dim;
    size_t _y_dim;
    size_t _total;

    int selected;

    glm::vec3 * forces;
    PointMass * pointMasses;
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
    bool is_selected(int i);

public:
    Cloth(size_t x, size_t y);

    void init(Shader & shader);

    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down);
    void drag_selected(glm::vec3 direction);

    void cleanup();

};

#endif // CLOTH_H