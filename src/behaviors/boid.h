#ifndef BOID_H
#define BOID_H

#include "../common.h"
#include "../scene.h"
#include "../entities/entity.h"

class BoidBehavior {
    Entity * entity;
    float perch_timer;
    // glm::vec3 velocity;

    glm::vec3 get_separation();
    glm::vec3 get_alignment();
    glm::vec3 get_cohesion();
public:
    bool perching;
    BoidBehavior(Entity * entity);
    ~BoidBehavior();

    void update(float dt);
    glm::vec3 bound_position();
};

#endif // BOID_H