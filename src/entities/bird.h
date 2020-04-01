#ifndef BIRD_H
#define BIRD_H

#include <vector>
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/model.h"
#include "../behaviors/boid.h"

class Bird: public Entity {
    Model * _model;
    BoidBehavior boid_behavior;
public:
    Bird(float radius);
    ~Bird();

    double time;

    bool test_ray(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance);; 
    void navigate_to(orientation_state target);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // BIRD_H