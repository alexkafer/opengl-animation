#include "entity.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>

#include "../common.h"
#include "../scene.h"

Entity::Entity() {
    t = 0.f;
    step_time = 0.f;

    _current_path = std::vector<glm::vec3>(1, glm::vec3(0.0f));
}

// Animated position
void Entity::calculate_animation() {
    t = 0.0f;

    if (_current_path.size() > 1) {
        _current_path.back() = _origin;

        float distance = glm::distance(_origin, _current_path.end()[-2]);
        step_time = distance / SPEED;
    } else {
        step_time = 0.0f;
    }
}

void Entity::update(float dt) {
    // _current_path always has the expected destination in front ([0]) 
    // and current position (_origin) greater than or equal to the back
    
    // If path is 1, thats just the current position and we're not moving 
    if (_current_path.size() > 1) {

        // Path smoothing
        // If there is a clear shot to the target, remove the in-between
        for (std::vector<glm::vec3>::iterator future_target = _current_path.begin(); future_target < _current_path.end() - 2; future_target++) {
            
            if (!Globals::scene->check_collisions(_origin, *future_target, this)) {
                _current_path.erase(future_target + 1, _current_path.end()-1);
                // _current_path.push_back(_origin);
                calculate_animation();
                break;
            }
        }

        // We are in the process of LERPing to the next point in the path 
        if (t < step_time ) {
            // LERP from the back to the second to back
            t += dt;
            _origin = glm::mix(_current_path.back(), _current_path[_current_path.size()-2], t / step_time);
        } else {
            // Made it to the end. _origin is now the second to back, so drop the old back
            if (t > step_time) _current_path.pop_back();
            // start a new animation if necessary
            calculate_animation();
        }
    }             
}

void Entity::reset() {
    t = 0;
    step_time = 0;
    _current_path.clear();
}

void Entity::navigate_to(glm::vec3 pos) {
    std::cout << "Navigating to " << glm::to_string(pos) << std::endl;
    _current_path = Globals::scene->find_path(_origin, pos, this);
    calculate_animation();
}

std::vector<glm::vec3> Entity::get_current_path() {
    return _current_path;
}

float Entity::get_radius() {
    return _radius;
}