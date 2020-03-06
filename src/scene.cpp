#include "scene.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"

const int NUM_MILESTONES = 100;

Scene::Scene () {
        entities = std::vector<Entity*>();
        roadmap = new Roadmap(NUM_MILESTONES);
    	// Initialize the shader (which uses glew, so we need to init that first).
        // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
        // the full path to this project's src/ directory.
        renderer = new Phong();
        check_gl_error();

        path_renderer = new LineRenderer();
        check_gl_error();

        particles = new Particles();
        check_gl_error();
}

Roadmap * Scene::get_roadmap() {
    return roadmap;
}

size_t Scene::add_entity(Entity * entity) {
    entities.push_back(entity);
    update_roadmap();
    return renderer->add_object(entity);
}

size_t Scene::add_renderable(Renderable * renderable) {
    return renderer->add_object(renderable);
}

void Scene::print_stats() {
    particles->print_stats();
}

void Scene::init() {
    srand (time(NULL));
}

void Scene::draw(float dt) {
    renderer->draw();
    path_renderer->draw(roadmap->get_milestones());
}

bool Scene::check_collision(glm::vec3 a, glm::vec3 b) {
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        if ((*t)->check_collision(a, b)) {
            return true;
        }
    }

    return false;
}

void Scene::update_roadmap() {
    // Update milestones
    roadmap->clear();

    float HI = 10.f;
    float LO = -10.f;

    int count = 0;

    while (count < NUM_MILESTONES) {
        glm::vec3 milestone (
            LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO))),
            0.5f,
            LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(HI-LO)))
        );

        if (!check_collision(milestone,milestone)) {
            roadmap->add_milestone(milestone);
            count++;
        }
    }
    std::cout << "Generated milestones" << std::endl;;
    const std::vector<glm::vec3> milestones = roadmap->get_milestones();

    for (size_t i = 0; i < milestones.size(); i++) {
        for (size_t j = 0; j < milestones.size(); j++) {
            if (i == j) continue;

            if (!check_collision(milestones.at(i), milestones.at(j))) {
                roadmap->add_edge(i, j, glm::distance(milestones.at(i), milestones.at(j)));
            }
        }
    }
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
    for (size_t i = 0; i < entities.size(); i++) {
        std::cout << entities.at(i) << std::endl;
        entities.at(i)->reset();
    }

    update_roadmap();
}

void Scene::cleanup() {
    renderer->cleanup();
    delete renderer;
}