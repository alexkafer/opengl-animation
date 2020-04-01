#ifndef BOX_H
#define BOX_H

#include <vector>
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/model.h"
#include "../behaviors/boid.h"

enum BoxSize {
    Wide,
    Single
};

class Box: public Entity {
    Model * _model;
    BoxSize _size;
public:
    Box(float radius, BoxSize size);
    ~Box();

    double time;

    void navigate_to(orientation_state target);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // BOX_H