#ifndef PLAYER_H
#define PLAYER_H

#include <vector>
#include "../utils/shader.h"

#include "../entities/entity.h"
#include "../geometry/model.h"
#include "../behaviors/follow_path.h"

class Player: public Entity {
    Model * _model;
    FollowPathBehavior path_behavior;
public:
    Player(float radius);
    ~Player();

    double time;

    bool check_collision(const orientation_state & a, const orientation_state & b, float body_radius);
    std::vector<orientation_state> get_current_path();

    void navigate_to(orientation_state pos);

    void init(Shader & shader);
    void reset();
    void update(float dt);
    void draw(Shader & shader);
    void cleanup();

};

#endif // PLAYER_H