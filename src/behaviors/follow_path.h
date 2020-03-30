#ifndef FOLLOW_PATH_H
#define FOLLOW_PATH_H

#include "../common.h"
#include "../entities/entity.h"

#include <vector>

class FollowPathBehavior {
    
    // Path Finding
    Entity * entity;
    std::vector<orientation_state> _current_path;

    float t_animation = 0.0f;
    float total_animation = 0.0f;
    glm::quat start_rotation;
    glm::quat target_rotation;
public:
    FollowPathBehavior(Entity * entity);
    ~FollowPathBehavior();

    void calculate_animation();
    bool is_animating();

    std::vector<orientation_state> get_current_path();
    void navigate_to(orientation_state pos);

    void reset();

    void update(float dt);
};

#endif // FOLLOW_PATH_H