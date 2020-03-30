
#include "bird.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Bird::Bird(float radius): Entity(glm::vec3(radius), glm::vec3(0.f, 0.f, -1.f)) {
    _radius = radius;
    _direction = glm::vec3(0.f, 0.f, 1.f);

    orientation_state state = Globals::scene->get_random_orientation();
    set_position(state.first);
    set_direction(state.second);
}

Bird::~Bird() {
    delete _model;
}

bool Bird::check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) {
    return false;
}

void Bird::init(Shader & shader) {
    _model = Globals::scene->load_model("bird/bird.fbx");
    Globals::scene->add_renderable(_model, this);
}

void Bird::reset() {
    Entity::reset();
}

void Bird::update(float dt) {
    Entity::update(dt);

    time += dt;

    _model->update_animation(time);
}

void Bird::draw(Shader & shader) {}

void Bird::cleanup() {}
