#ifndef BALL_H
#define BALL_H

#include <vector>
#include <list>
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

    // Path Finding
    glm::vec3 target;
    std::list<glm::vec3> current_path;
    
    // Animation
    glm::vec3 prev_step;
    glm::vec3 next_step;
    float t, step_time;
    

public:
    Ball(float radius);
    void animate_position(glm::vec3 pos);
    void navigate_to(glm::vec3 target); 
    bool check_collision(glm::vec3 a, glm::vec3 b);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // BALL_H