#ifndef BOID_H
#define BOID_H

#include "../common.h"

class BoidBehavior {
    
    glm::vec3 get_separation();
    glm::vec3 get_alignment();
    glm::vec3 get_cohesion();
public:
    BoidBehavior();
    ~BoidBehavior();

    glm::vec3 get_target_velocity();
};

#endif // BOID_H