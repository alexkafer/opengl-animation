#ifndef BALL_H
#define BALL_H

#include <vector>
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/sphere.h"

class Ball: public Entity{
    Sphere _sphere;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

public:
    Ball(float radius);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // BALL_H