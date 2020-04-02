
#include "bird.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Bird::Bird(float radius): Entity(BirdEntity, glm::vec3(1.f, 0.f, 0.f)), boid_behavior(this) {
    _radius = radius;
}

Bird::~Bird() {
    delete _model;
}



void Bird::init(Shader & shader) {
    _model = Globals::scene->load_model("bird/boid.fbx", glm::vec3(0.005f));
    Globals::scene->add_renderable(_model, this);
}

void Bird::navigate_to(orientation_state state) {
    set_position(state.position);
}

bool Bird::test_ray(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance) {
	return false;
}

void Bird::reset() {
    Entity::reset();
}

void Bird::update(float dt) {
    // Entity::update(dt);  This would add gravity

    boid_behavior.update(dt);

    if (!boid_behavior.perching) {
        // time += dt;
        // _model->update_animation(0, time / 100.f);
    }
}

void Bird::draw(Shader & shader) {}

void Bird::cleanup() {}
