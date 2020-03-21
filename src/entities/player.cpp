
#include "player.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"

Player::Player(float radius) {
    _radius = radius;

    std::stringstream model_ss; model_ss << MY_MODELS_DIR << "Firehydrant/firehydrant.obj";
    _model = new Model(model_ss.str(), false);
}

bool Player::check_collision(const glm::vec3 & a, const glm::vec3 & b, float radius_offset) {
    return false;
}

void Player::init(Shader & shader) {
    _model->init(shader);
}

void Player::reset() {
    Entity::reset();
}

void Player::update(float dt) {
    Entity::update(dt);
}

void Player::draw(Shader & shader) {
    _model->draw(shader);
}

void Player::cleanup() {
    _model->cleanup();

    delete _model;
}
