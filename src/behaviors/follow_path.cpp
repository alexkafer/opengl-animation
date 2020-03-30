#include "follow_path.h"

#include "../common.h"
#include "../scene.h"

FollowPathBehavior::FollowPathBehavior(Entity * entity) {
    this->entity = entity;
    reset();
}

FollowPathBehavior::~FollowPathBehavior() {}

std::vector<orientation_state> FollowPathBehavior::get_current_path() {
    return _current_path;
}

void FollowPathBehavior::navigate_to(orientation_state pos) {
    _current_path = Globals::scene->find_path(pos, entity);
    calculate_animation();
} 

// Animated position
void FollowPathBehavior::calculate_animation() {
    t_animation = 0.0f;

    if (_current_path.size() > 1) {
        _current_path.back() = {entity->get_position(), entity->get_direction()};

        orientation_state target = _current_path.end()[-2];
        glm::vec3 diff = entity->get_position() - target.first;

        float distance = glm::length(diff);
        total_animation = distance / SPEED;
    } else {
        total_animation = 0.0f;
    }
}


void FollowPathBehavior::update(float dt) {
    // _current_path always has the expected destination in front ([0]) 
    // and current position (_origin) greater than or equal to the back
    
    // If path is 1, thats just the current position and we're not moving 
    if (_current_path.size() > 1) {

        // Path smoothing
        // If there is a clear shot to the target, remove the in-between
        for (std::vector<orientation_state>::iterator future_target = _current_path.begin(); future_target < _current_path.end() - 2; future_target++) {
            
            if (!Globals::scene->check_collisions({entity->get_position(), entity->get_direction()}, *future_target, entity)) {
                _current_path.erase(future_target + 1, _current_path.end()-1);
                // _current_path.push_back(_origin);
                calculate_animation();
                break;
            }
        }

        // We are in the process of LERPing to the next point in the path 
        if (t_animation < total_animation ) {
            // LERP from the back to the second to back
            t_animation += dt;
            entity->_origin = glm::mix(_current_path.back().first, _current_path[_current_path.size()-2].first, t_animation / total_animation);

            // SLERP the rotation
            glm::vec3 start_orientation = _current_path.back().second;
            glm::vec3 end_orientation = _current_path[_current_path.size()-2].second;

            float start_angle = atan2( start_orientation.x, start_orientation.z);
            glm::quat start_rotation = glm::quat(cos(start_angle/2.f), 0.f, sin(start_angle/2.f), 0.f);  

            float end_angle = atan2( end_orientation.x, end_orientation.z);
            glm::quat end_rotation = glm::quat(cos(end_angle/2.f), 0.f, sin(end_angle/2.f), 0.f);

            entity->_rotation = glm::mix(start_rotation, end_rotation, t_animation / total_animation);
        } else {
            // Made it to the end. _origin is now the second to back, so drop the old back
            if (t_animation > total_animation) _current_path.pop_back();
            // start a new animation if necessary
            calculate_animation();
        }
    }
}

void FollowPathBehavior::reset() {
    _current_path = std::vector<orientation_state>();
    _current_path.push_back(std::make_pair(entity->get_position(), entity->get_direction()));

    t_animation = 0.0f;
    total_animation = 0.0f;

}