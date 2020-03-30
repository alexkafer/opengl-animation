#ifndef ENTITY_H
#define ENTITY_H

#include <vector>
#include "../renderers/renderable.h"

static const float SPEED = 6.f; // 5 meters (units) per second

class Entity: public Renderable {
protected:
    float _radius;

    // Rotation Animation
    float rotation_t, rotation_total;

    glm::vec3 _velocity;
    // glm::vec3 _acceleration;
public:
    Entity();
    Entity(glm::vec3 scale, glm::vec3 direction);
    
    // void calculate_animation();
    virtual void navigate_to(orientation_state target); 

    virtual void reset(); 
    virtual void update(float dt); 
    virtual bool check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) = 0; 

    std::vector<orientation_state> get_current_path();
    float get_radius();

    friend class FollowPathBehavior;
    friend class BoidBehavior;
};

#endif // ENTITY_H