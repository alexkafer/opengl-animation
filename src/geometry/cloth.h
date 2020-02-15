#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "../common.h"
#include "../shader.hpp"

#include "draggable.h"
#include "sphere.h"

struct PointMass : public Draggable {
    glm::vec3 position;
    glm::vec3 velocity;

    PointMass(const glm::vec3 &_position) 
        : position(_position), velocity(0.f) {} 

    glm::vec3 get_position() {
        return position;
    }

    void set_position(glm::vec3 p) {
        position = p;
    }
};

class Cloth {
    size_t _x_dim;
    size_t _y_dim;
    std::vector<PointMass> pointMasses;
    std::vector<std::pair<size_t, size_t>> edges;

    Sphere ball;

    GLuint vao;
    GLuint ball_vbo;
    GLuint ball_ibo;

    void update_physics(float dt);

public:
    Cloth(size_t x, size_t y);

    void init(mcl::Shader & shader);

    void update(float dt);
    void draw(mcl::Shader & shader);
    Draggable* find_draggable(glm::vec3 origin, glm::vec3 direction);

    void cleanup();

};

#endif // CLOTH_H