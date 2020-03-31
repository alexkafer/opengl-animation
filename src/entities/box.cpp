
#include "box.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/intersect.hpp>

#include "../utils/roadmap.h"
#include "../scene.h"

Box::Box(float radius, BoxSize size): Entity(ObstacleEntity, glm::vec3(radius * (size == Wide ? 0.04f: 18.f)), glm::vec3(1.f, 0.f, 0.f)) {
    _radius = radius;
    _size = size;
   

    orientation_state state = Globals::scene->get_random_orientation(true);
    set_position(state.first);
    set_direction(state.second);
}



Box::~Box() {
    delete _model;
}

bool Box::check_collision(const orientation_state & a, const orientation_state & b, float radius_offset) {

    if (_size == Wide) {
        //  // Collision x-axis?
        // bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        //     two.Position.x + two.Size.x >= one.Position.x;
        // // Collision y-axis?
        // bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        //     two.Position.y + two.Size.y >= one.Position.y;
        // // Collision only if on both axes
        // return collisionX && collisionY;
    }
    return false;
}

void Box::init(Shader & shader) {

    if (_size == Wide) {
        _model = Globals::scene->load_model("box/metal.fbx");
        Globals::scene->add_renderable(_model, this);
    } else {
        _model = Globals::scene->load_model("box/wooden_crate/scene.gltf");
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
