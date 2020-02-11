#ifndef CLOTH_H
#define CLOTH_H

#include <vector>
#include "../common.h"
#include "../shader.hpp"

#include "sphere.h"

struct PointMass {
    glm::vec3 position;
    glm::vec3 velocity;
    size_t connected_to;

    PointMass(const glm::vec3 &_position, size_t _connected_to) 
        : position(_position), velocity(0.f),connected_to(_connected_to) {} 
};

class Cloth {
    std::vector<PointMass> pointMasses;

    Sphere ball;

    GLuint vao;
    GLuint ball_vbo;
    GLuint ball_ibo;
public:
    Cloth(size_t points);

    void init(mcl::Shader & shader);

    void update(float dt);
    void draw(mcl::Shader & shader);

    void cleanup();

};

#endif // CLOTH_H