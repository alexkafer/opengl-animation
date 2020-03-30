#include "scene.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

const int NUM_MILESTONES = 50;

Scene::Scene () {
        entities = std::vector<Entity*>();
        roadmap = new Roadmap(NUM_MILESTONES);
    	// Initialize the shader (which uses glew, so we need to init that first).
        // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
        // the full path to this project's src/ directory.
        renderer = new Phong();
        check_gl_error();

        path_renderer = new PathRenderer();
        check_gl_error();

        particles = new Particles();
        check_gl_error();

        world = new World(25.f, 25.f);
}

Model * Scene::load_model(const std::string path) {
    map<std::string, Model *>::iterator it = loaded_models.find(path);

    if (it != loaded_models.end()) {
        std::cout << "Re-using " << path << std::endl;
        return it->second;
    } else {
        std::cout << "Loading " << path << std::endl;
        std::stringstream model_ss; model_ss << MY_MODELS_DIR << path;
        Model * model = new Model(model_ss.str(), false);
        loaded_models.insert({path, model});

        return model;
    }
 
}

void Scene::add_entity(Entity * entity) {
    std::cout << "Added entity" << std::endl;
    entities.push_back(entity);
    renderer->add_object(entity);
}

void Scene::add_renderable(Renderable * renderable, Renderable * parent) {
    renderer->add_object(renderable, parent);
}

void Scene::add_light(glm::vec4 pos, glm::vec3 strength, float attenuation, float ambient) {
    renderer->add_light(pos, strength, attenuation, ambient);
}

void Scene::print_stats() {
    particles->print_stats();
}
 
void Scene::init() {
    srand (time(NULL));

    renderer->add_object(world);
}

void Scene::draw(float dt) {
    renderer->draw();
    
    path_renderer->draw_milestones(roadmap->get_milestones(), glm::vec4(0.f, 0.f, 0.f, 0.5f));
    // path_renderer->draw_edges(roadmap->get_edges(), glm::vec4(0.f, 0.f, 0.f, 0.1f));

    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        std::vector<orientation_state> path = (*t)->get_current_path();

        std::vector<glm::vec3> edges; 

        for (int i = 1; i < path.size(); i++) 
        {
            edges.push_back(path.at(i-1).first);
            edges.push_back(path.at(i).first);
        } 

        path_renderer->draw_edges(edges, glm::vec4(1.f, 0.f, 0.f, 0.5f));
    }
}

// -2.543759, 0.500000, 7.050190   0
// -7.433999, 0.500000, -3.219673   1
// 6.968929, 0.500000, 6.793032  2

bool Scene::check_collisions(const orientation_state & a, const orientation_state & b, Entity * entity) {
    float body_radius = entity->get_radius();
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        if ((*t) == entity) continue;

        if ((*t)->check_collision(a, b, body_radius)) {
            return true;
        }
    }

    return false;
}

orientation_state Scene::get_random_orientation() {
    glm::vec3 center = world->get_random_point();
    glm::vec3 wall = glm::sphericalRand(1.f);
    return { center, wall - center };
}

void Scene::populate_roadmap(Entity * entity) {
    // Update milestones
    int count = 0;

    while (count < NUM_MILESTONES-2) {
        orientation_state milestone = get_random_orientation();

        if (!check_collisions(milestone, milestone, entity)) {
            roadmap->add_milestone(milestone);
            count++;
        }
    }

    std::cout << "Generated milestones" << std::endl;;
    const std::vector<std::pair<glm::vec3,glm::vec3>> milestones = roadmap->get_milestones();

    for (size_t i = 0; i < milestones.size(); i++) {
        for (size_t j = 0; j < milestones.size(); j++) {
            if (i == j) continue;

            if (!check_collisions(milestones.at(i), milestones.at(j), entity)) {
                roadmap->add_edge(i, j, glm::distance(milestones.at(i).first, milestones.at(j).first));
            }
        }
    }
}

std::vector<orientation_state> Scene::find_path(orientation_state destination, Entity * entity) {
    // Find the milestone closest to start
    roadmap->clear();

    orientation_state start = { entity->get_position(), entity->get_direction()};

    std::vector<orientation_state> path;

    if (!check_collisions(start, destination, entity)) {
        path.push_back(destination);
        path.push_back(start);
        return path;
    }
    
    int start_id = roadmap->add_milestone(start);
    int finish_id = roadmap->add_milestone(destination);

    populate_roadmap(entity);
    
    return roadmap->dijkstra_path(start_id, finish_id);
}

void Scene::update(float dt) {
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        (*t)->update(fmin(dt, 0.02f));
    }

    particles->update(dt, glm::vec3(-1.37f, 1.15f, -8.f), .6f);
}

glm::vec3 Scene::find_collision(glm::vec3 origin, glm::vec3 direction) {
    // assuming vectors are all normalized
    // Dot floor normal (up) and ray direction
    float denom = glm::dot(glm::vec3(0.f, 1.f, 0.f), direction); 

    if (abs(denom) > 1e-6) { 
        float t = glm::dot(-origin, glm::vec3(0.f, 1.f, 0.f)) / denom; 
        if (t >= 0) {
            return origin + t * direction;
        }
    } 
 
    throw 1;
}

void Scene::interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down) {
    // std::cout << "Test" << std::endl;
    //fluid->interaction(origin, direction, mouse_down);
    // cloth->interaction(origin, direction, mouse_down);
}

void Scene::key_down(int key) {}

void Scene::reset() {
    for (size_t i = 0; i < entities.size(); i++) {
        std::cout << entities.at(i) << std::endl;
        entities.at(i)->reset();
    }

    roadmap->clear();
}

void Scene::cleanup() {
    renderer->cleanup();
    delete renderer;
}