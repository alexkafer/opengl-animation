#ifndef ENTITY_H
#define ENTITY_H

#include "../renderers/renderable.h"

class Entity: public Renderable {
public:
    virtual void reset() = 0; 
    virtual void update(float dt) = 0; 
    virtual bool check_collision(glm::vec3 position) = 0; 
};

#endif // ENTITY_H