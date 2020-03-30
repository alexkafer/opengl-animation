#ifndef BOID_H
#define BOID_H

#include "../common.h"
#include "../scene.h"
#include "../entities/entity.h"

class BoidBehavior {
    Entity * entity;
    // glm::vec3 velocity;

    glm::vec3 get_separation();
    glm::vec3 get_alignment();
    glm::vec3 get_cohesion();
public:
    BoidBehavior(Entity * entity);
    ~BoidBehavior();

    void update(float dt);
};

#endif // BOID_H