#include "scene.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"

Scene::Scene () {
        entities = std::vector<Entity*>();
    	// Initialize the shader (which uses glew, so we need to init that first).
        // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
        // the full path to this project's src/ directory.
        renderer = new Phong();
        check_gl_error();

        particles = new Particles();
        check_gl_error();

        add_renderable(new Floor());
        
        ball = new Ball(0.5f);
        ball->set_position(glm::vec3(-9.f, 0.5f, -9.f));
        ball->animate_position(glm::vec3(9.f, 0.5f, 9.f));
        add_entity(ball);

        obstacle = new Ball(2.f);
        obstacle->set_position(glm::vec3(0.f, 0.f, 0.f));
        add_entity(obstacle);

        // fluid = new Fluid(25, 25, 25);
        
        check_gl_error();
}

size_t Scene::add_entity(Entity * entity) {
    entities.push_back(entity);
    return renderer->add_object(entity);
}

size_t Scene::add_renderable(Renderable * renderable) {
    return renderer->add_object(renderable);
}

void Scene::print_stats() {
    particles->print_stats();
}

void Scene::init() {}

void Scene::draw(float dt) {
    renderer->draw();
}

void Scene::update(float dt) {
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        (*t)->update(fmin(dt, 0.02f));
    }

    particles->update(dt, glm::vec3(-1.37f, 1.15f, -8.f), .6f);
}

void Scene::interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down) {
    //fluid->interaction(origin, direction, mouse_down);
    // cloth->interaction(origin, direction, mouse_down);
}

void Scene::key_down(int key) {}

void Scene::reset() {
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        (*t)->reset();
    }
}

void Scene::cleanup() {
    renderer->cleanup();
    delete renderer;
}