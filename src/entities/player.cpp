
#include "player.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Player::Player(float radius): Entity(glm::vec3(0.05f)) {
    _radius = radius;
}

Player::~Player() {
    delete _model;
}

bool Player::check_collision(const glm::vec3 & a, const glm::vec3 & b, float radius_offset) {
    return false;
}

void Player::init(Shader & shader) {
    std::stringstream model_ss; model_ss << MY_MODELS_DIR << "Zombie.fbx";
    _model = new Model(model_ss.str(), false);
    Globals::scene->add_renderable(_model, this);
}

void Player::reset() {
    Entity::reset();
}

void Player::update(float dt) {
    Entity::update(dt);
}

void Player::draw(Shader & shader) {}

void Player::cleanup() {}
