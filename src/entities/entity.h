#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "../renderers/renderable.h"

static const float SPEED = 10.f; // 2 meters (units) per second

class Entity: public Renderable {
protected:
    float _radius;

    // Path Finding
    glm::vec3 _target;
    std::vector<glm::vec3> _current_path;

    // Animation
    glm::vec3 prev_step;
    glm::vec3 next_step;
    float t, step_time;

public:
    void animate_position(glm::vec3 pos);
    void navigate_to(glm::vec3 target); 

    virtual void reset() = 0; 
    virtual void update(float dt) = 0; 
    virtual bool check_collision(const glm::vec3 & a, const glm::vec3 & b, float radius_offset) = 0; 

    std::vector<glm::vec3> get_current_path();
    float get_radius();
};

#endif // ENTITY_H