#ifndef BALL_H
#define BALL_H

#include "../common.h"
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/sphere.h"

class Ball: public Entity {
    float _radius;
    Sphere _sphere;

    GLuint vao;
    GLuint vbo;
    GLuint ibo;

    // Animation
    glm::vec3 start;
    glm::vec3 target;
    float t, total_time;
    

public:
    Ball(float radius);
    void animate_position(glm::vec3 pos); 

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // BALL_H