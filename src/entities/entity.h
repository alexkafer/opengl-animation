#ifndef ENTITY_H
#define ENTITY_H

#include "../renderers/renderable.h"

class Entity: public Renderable {
public:
    virtual void reset() = 0; 
    virtual void update(float dt) = 0; 
};

#endif // ENTITY_H