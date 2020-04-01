
#include "bird.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Bird::Bird(float radius): Entity(BirdEntity, glm::vec3(0.f, 0.f, 0.f)), boid_behavior(this) {
    _radius = radius;

    orientation_state state = Globals::scene->get_random_orientation(false);
    // set_position(state.first);
    // set_direction(state.second);
}

Bird::~Bird() {
    delete _model;
}

bool Bird::check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) {
     if (a == b) {
        float x = (a.first.x - _origin.x);
        float y = (a.first.y - _origin.y);
        float z = (a.first.z - _origin.z);
        return (x*x + y*y + z*z) < ((_radius + radius_offset) * (_radius + radius_offset) * (_radius + radius_offset));
    } else {
        float d = glm::length(glm::cross(_origin - a.first, _origin - b.first)) / glm::length(b.first - a.first);
        return d <= (_radius + radius_offset);
    }
}

void Bird::init(Shader & shader) {
    _model = Globals::scene->load_model("bird/bird.fbx", glm::vec3(0.005f));
    Globals::scene->add_renderable(_model, this);
}

void Bird::navigate_to(orientation_state pos) {
    std::cout << "Not exactly sure how to get there. " << std::endl;
    throw 0;
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
