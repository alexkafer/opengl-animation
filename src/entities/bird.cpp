
#include "bird.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Bird::Bird(float radius): Entity(BirdEntity, glm::vec3(0.f, 0.f, 1.f)), boid_behavior(this) {
    _radius = radius;
}

Bird::~Bird() {
    delete _model;
}



void Bird::init(Shader & shader) {
    _model = Globals::scene->load_model("bird/Eagle.fbx", glm::vec3(0.005f));
    Globals::scene->add_renderable(_model, this);
}

void Bird::navigate_to(orientation_state pos) {
    set_position(pos.first);
}

bool Bird::test_ray(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance) {
	return false;
}

void Bird::reset() {
    Entity::reset();
}

void Bird::update(float dt) {
    Entity::update(dt);

    boid_behavior.update(dt);

    if (!boid_behavior.perching) {
        time += dt;
        _model->update_animation(1, time);
    }
}

void Bird::draw(Shader & shader) {}

void Bird::cleanup() {}
