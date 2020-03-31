
#include "player.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Player::Player(float radius): Entity(PlayerEntity, glm::vec3(0.04f), glm::vec3(0.f, 0.f, 1.f)), path_behavior(this) {
    _radius = radius;
}

Player::~Player() {
    delete _model;
}

bool Player::check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) {
    return false;
}

void Player::init(Shader & shader) {
    _model = Globals::scene->load_model("character/walking.fbx");
    Globals::scene->add_renderable(_model, this);
}


void Player::reset() {
    Entity::reset();
}

void Player::update(float dt) {
    Entity::update(dt);
    path_behavior.update(dt);

    std::cout << "Player bounding box is: " << glm::to_string(bbox.first) << " to " << glm::to_string(bbox.second)<< std::endl;

    if (path_behavior.is_animating()) {
        time += dt;
        _model->update_animation(time);
        // update_bounding_box();

        // bbox.first = glm::max(bbox.first, _model->get_bounding_box().first);
        // bbox.second = glm::min(bbox.second, _model->get_bounding_box().second);
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
