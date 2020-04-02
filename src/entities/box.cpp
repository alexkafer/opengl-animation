
#include "box.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Box::Box(float radius, BoxSize size): Entity(ObstacleEntity, glm::vec3(1.f, 0.f, 0.f)) {
    _radius = radius;
    _size = size;
}

Box::~Box() {
    delete _model;
}

void Box::navigate_to(orientation_state pos) {
    set_position(pos.position);
}

void Box::init(Shader & shader) {

    if (_size == Wide) {
        _model = Globals::scene->load_model("box/metal.fbx", glm::vec3(1.f));
        Globals::scene->add_renderable(_model, this);
    } else {
        _model = Globals::scene->load_model("box/wooden_crate/scene.gltf", glm::vec3(10.f));
        Globals::scene->add_renderable(_model, this);
    }
    
}

void Box::reset() {
    Entity::reset();
}

void Box::update(float dt) {
    Entity::update(dt);

    // time += dt;
    // _model->update_animation(time);
}

void Box::draw(Shader & shader) {}

void Box::cleanup() {}
