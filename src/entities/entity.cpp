#include "entity.h"

#include <iostream>

#include "../common.h"
#include "../scene.h"

// Animated position
void Entity::animate_position(glm::vec3 pos) {
    prev_step = _origin;
    next_step = pos;
    t = 0;
    float distance = glm::distance(prev_step, next_step);
    step_time = distance / SPEED;
}

void Entity::navigate_to(glm::vec3 pos) {
    _target = pos;
    _current_path = Globals::scene->find_path(_origin, pos, this);
}

std::vector<glm::vec3> Entity::get_current_path() {
    return _current_path;
}

float Entity::get_radius() {
    return _radius;
}