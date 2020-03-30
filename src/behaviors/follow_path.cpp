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
        glm::vec3 partial_direction = glm::rotate(entity->get_rotation(), entity->get_direction());
        entity->_direction = partial_direction;

        _current_path.back() = {entity->get_position(), partial_direction};

        orientation_state start_orientation = _current_path.back();
        orientation_state target_orientation = _current_path.end()[-2];

        glm::vec3 diff = start_orientation.first - target_orientation.first;

        start_rotation = entity->calculate_rotation(start_orientation.second);
        target_rotation = entity->calculate_rotation(target_orientation.second);
        
        float distance = glm::length(diff);
        total_animation = distance / SPEED;
    } else {
        total_animation = 0.0f;
    }
}

glm::quat RotateTowards(glm::quat q1, glm::quat q2, float maxAngle){

	if( maxAngle < 0.001f ){
		// No rotation allowed. Prevent dividing by 0 later.
		return q1;
	}

	float cosTheta = glm::dot(q1, q2);

	// q1 and q2 are already equal.
	// Force q2 just to be sure
	if(cosTheta > 0.9999f){
		return q2;
	}

	// Avoid taking the long path around the sphere
	if (cosTheta < 0){
	    q1 = q1*-1.0f;
	    cosTheta *= -1.0f;
	}

    // calculate interpolation factors
    // if (abs(1-cosTheta) < 0.01f) {
    //     // quaternions are nearly parallel, so use linear interpolation
    //     r = 1 - f;
    // }

	float angle = acos(cosTheta);

	// If there is only a 2&deg; difference, and we are allowed 5&deg;,
	// then we arrived.
	if (angle < maxAngle){
		return q2;
	}

	float fT = maxAngle / angle;
	angle = maxAngle;

	glm::quat res = (sin((1.0f - fT) * angle) * q1 + sin(fT * angle) * q2) / sin(angle);
	res = glm::normalize(res);
	return res;
}

bool FollowPathBehavior::is_animating() {
    return t_animation < total_animation;
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
        if (is_animating()) {
    
            t_animation += dt;

            // LERP from the back to the second to back
            entity->_origin = glm::mix(_current_path.back().first, _current_path[_current_path.size()-2].first, t_animation / total_animation);
            
            // SLERP the rotation
            float faster_rotation = 1.f; // 4.f looks good
            entity->set_rotation(RotateTowards(entity->_rotation, target_rotation, M_PI_2 * dt));
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