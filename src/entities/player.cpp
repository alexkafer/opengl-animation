
#include "player.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

static const float height = 2.f;

Player::Player(float radius): Entity(PlayerEntity, glm::vec3(0.f, 0.f, 1.f)), path_behavior(this) {
    _radius = radius;

    set_scale(glm::vec3(0.08f));
}

Player::~Player() {
    delete _model;
}

// bool Player::test_ray(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance) {
//     Cylinder shape(_origin, 1.5f, _radius);
// 	return shape.intersect(ray_origin, ray_direction, intersection_distance);
// }

// bool Player::check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) {
//     if (a == b) {
//         float x = (a.first.x - _origin.x);
//         float y = (a.first.y - _origin.y);
//         float z = (a.first.z - _origin.z);
//         if (y > (height+ radius_offset ) || y < -radius_offset) return false;
//         return (x*x + z*z) < ((_radius + radius_offset) * (_radius + radius_offset));
//     } else {
//         float d = glm::length(glm::cross(_origin - a.first, _origin - b.first)) / glm::length(b.first - a.first);
//         return d <= (_radius + radius_offset);
//     }
// }

void Player::init(Shader & shader) {
    _model = Globals::scene->load_model("character/walking.fbx", glm::vec3(0.5f));
    Globals::scene->add_renderable(_model, this);
}


void Player::reset() {
    Entity::reset();
}

void Player::update(float dt) {
    Entity::update(dt);
    path_behavior.update(dt);

    if (path_behavior.is_animating()) {
        time += dt;
        _model->update_animation(0, time);
    }
}

std::vector<orientation_state> Player::get_current_path() {
    return path_behavior.get_current_path();
}

void Player::navigate_to(orientation_state pos) {
    std::cout << "Navigating to " << glm::to_string(pos.first) << std::endl;

    path_behavior.navigate_to(pos);
}

void Player::draw(Shader & shader) {}

void Player::cleanup() {}
