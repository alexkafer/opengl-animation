#include "entity.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../common.h"
#include "../scene.h"



Entity::Entity(glm::vec3 scale, glm::vec3 direction): Renderable(scale, direction) {
    _velocity = glm::vec3(0.f);
}

Entity::Entity(): Entity(glm::vec3(1.f), glm::vec3(1.f, 0.f, 0.f)) {}

void Entity::update(float dt) {
    // Update the current rotation based on direction the entity is facing
    // float current_angle = atan2( _direction.x, _direction.z);
    // _rotation = glm::quat(cos(current_angle/2.f), 0.f, sin(current_angle/2.f), 0.f);  
}

void Entity::reset() {}

void Entity::navigate_to(orientation_state pos) {
    std::cout << "Not exactly sure how to get there. I'm not real. " << std::endl;
    throw 0;
}

std::vector<orientation_state> Entity::get_current_path() {
    return {{_origin + _direction, _direction}, {_origin, _direction}};
}

float Entity::get_radius() {
    return _radius;
}