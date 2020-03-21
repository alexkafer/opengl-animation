#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/model.h"

class Player: public Entity {
    Model * _model;
public:
    Player(float radius);
    ~Player();

    bool check_collision(const glm::vec3 & a, const glm::vec3 & b, float body_radius);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // PLAYER_H