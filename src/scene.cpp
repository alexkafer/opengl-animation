#include "scene.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/random.hpp>

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

        world = new World(100.f, 30.f, 100.f);
}

Model * Scene::load_model(const std::string path, const glm::vec3 & scale) {
    map<std::string, Model *>::iterator it = loaded_models.find(path);
    bool should_reuse = false;
    if (should_reuse && it != loaded_models.end()) {
        std::cout << "Re-using " << path << std::endl;
        return it->second;
    } else {
        std::cout << "Loading " << path << std::endl;
        std::stringstream model_ss; model_ss << MY_MODELS_DIR << path;
        Model * model = new Model(model_ss.str(), scale, false);
        loaded_models.insert({path, model});

        return model;
    }
 
}

void Scene::add_entity(Entity * entity) {
    std::cout << "Added entity of type: " << entity->get_type() << std::endl;
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
    
    path_renderer->draw_graph(glm::vec4(0.f, 0.f, 0.f, 0.5f));

    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        std::vector<orientation_state> path = (*t)->get_current_path();

        std::vector<glm::vec3> edges; 

        for (int i = 1; i < path.size(); i++) 
        {
            edges.push_back(path.at(i-1).first);
            edges.push_back(path.at(i).first);
        } 

        path_renderer->draw_path(edges, glm::vec4(1.f, 0.f, 0.f, 0.5f));

        
        
        if ((*t) == selected_entity) {
            path_renderer->draw_bounding_box((*t), glm::vec4(1.f, 0.f, 0.f, 0.5f));
        } else {
            path_renderer->draw_bounding_box((*t), glm::vec4(1.f, 1.f, 1.f, 0.1f));
        }
        
        check_gl_error();
    }
}

// -2.543759, 0.500000, 7.050190   0
// -7.433999, 0.500000, -3.219673   1
// 6.968929, 0.500000, 6.793032  2

bool Scene::check_collisions(const orientation_state & a, const orientation_state & b, Entity * entity) {
    float body_radius = entity->get_radius();
    std::vector<Entity*> nearby = get_nearby_entities(entity, 10.f);

    for(auto t=nearby.begin(); t!=nearby.end(); ++t) {
        if ((*t) == entity) continue;

        if ((*t)->check_collision(a, b, body_radius)) {
            return true;
        }
    }

    return false;
}

orientation_state Scene::get_random_orientation(bool ground) {
    glm::vec3 center = world->get_random_point(ground);

    if (ground) {
        glm::vec2 wall = glm::diskRand(1.f);
        return { center, glm::vec3(wall.x, 0.f, wall.y) };
    } else {
        glm::vec3 wall = glm::sphericalRand(1.f);
        return { center, wall };
    }
}

void Scene::populate_roadmap(Entity * entity) {
    // Update milestones
    int count = 0;

    while (count < NUM_MILESTONES-2) {
        orientation_state milestone = get_random_orientation(true);

        if (!check_collisions(milestone, milestone, entity)) {
            roadmap->add_milestone(milestone);
            count++;
        }
    }

    std::cout << "Generated milestones" << std::endl;;
    const std::vector<orientation_state> milestones = roadmap->get_milestones();

    for (size_t i = 0; i < milestones.size(); i++) {
        for (size_t j = 0; j < milestones.size(); j++) {
            if (i == j) continue;

            if (!check_collisions(milestones.at(i), milestones.at(j), entity)) {
                roadmap->add_edge(i, j, glm::distance(milestones.at(i).first, milestones.at(j).first));
            }
        }
    }
    // Let the roadmap know we're done, so it should compute the renderable graph
    path_renderer->init_graph(milestones, roadmap->get_edges());
}

std::vector<orientation_state> Scene::find_path(orientation_state destination, Entity * entity) {
    // Find the milestone closest to start
    roadmap->clear();

    orientation_state start = { entity->get_position(), entity->get_direction()};

    std::vector<orientation_state> path;

    if (!check_collisions(start, destination, entity)) {
        path.push_back({destination.first, destination.first - start.first});
        path.push_back(start);
        return path;
    }
    
    int start_id = roadmap->add_milestone(start);
    int finish_id = roadmap->add_milestone(destination);

    populate_roadmap(entity);
    
    return roadmap->a_star_path(start_id, finish_id);
    // return roadmap->dijkstra_path(start_id, finish_id);
}

void Scene::update(float dt) {
    for(auto t=entities.begin(); t!=entities.end(); ++t) {
        (*t)->update(fmin(dt, 0.02f));
    }

    particles->update(dt, glm::vec3(-1.37f, 1.15f, -8.f), .6f);
}

// Finds the collision for the floor to know where to target our walking dude
glm::vec3 Scene::find_target(glm::vec3 origin, glm::vec3 direction) {
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


std::vector<Entity*> Scene::get_nearby_entities(Entity * nearby, float radius) {
    return entities;
}

void Scene::interaction(glm::vec3 origin, glm::vec3 direction) {

    if (!Globals::mouse_down) return;

    Entity * closest_entity = nullptr;
    float closest_distance = 100000.f;

    for (size_t i = 0; i < entities.size(); i++) {
        float distance;
        if (entities[i]->test_ray(origin, direction, distance)) {
            std::cout << "Found: " << entities[i]->get_type() << std::endl;
            closest_entity = entities[i];
            closest_distance = distance;
        }
    }

    if (closest_entity != nullptr) {
        selected_entity = closest_entity;
    } 

    // Guard
    if(selected_entity == nullptr) return;

    

    if (selected_entity->get_type() == PlayerEntity) {
        try
        {
            glm::vec3 target = find_target(Globals::eye_pos, direction);
            target.y = 0.f;

            std::cout << "New target: " << glm::to_string(target) << std::endl;
            selected_entity->navigate_to({target, glm::vec3(0.0)});

        } catch (...){}
    } else {
        selected_entity->set_position(origin + glm::distance(origin, selected_entity->get_position()) * direction);
    }
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