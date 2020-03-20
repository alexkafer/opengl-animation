#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "../renderers/renderable.h"

static const float SPEED = 10.f; // 2 meters (units) per second

class Entity: public Renderable {
protected:
    float _radius;

    // Path Finding
    std::vector<glm::vec3> _current_path;

    // Animation
    float t, step_time;

public:
    Entity();
    void calculate_animation();
    void navigate_to(glm::vec3 target); 

    virtual void reset(); 
    virtual void update(float dt); 
    virtual bool check_collision(const glm::vec3 & a, const glm::vec3 & b, float radius_offset) = 0; 

    std::vector<glm::vec3> get_current_path();
    float get_radius();
};

#endif // ENTITY_H