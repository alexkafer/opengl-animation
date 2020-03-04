#include "scene.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "utils/tiny_obj_loader.h"

#include "utils/text2d.h"

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
        
        table = new Model("table/", "table.obj", glm::vec3( 0.05f, 0.1f, 0.05f ));
        add_renderable(table);
        
        cloth = new Cloth(30, 30);
        add_entity(cloth);

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

void Scene::compute_physics(float dt){
     for(auto t=entities.begin(); t!=entities.end(); ++t) {
        (*t)->update(fmin(dt, 0.02f));
     }
    // cloth->update(fmin(dt, 0.02f));
    // fluid->update(dt);
    particles->update(dt, glm::vec3(-1.37f, 1.15f, -8.f), .6f);
}

void Scene::init()
{
    initText2D( "Arial.ttf" );
}

void Scene::draw(float dt) {

    // // glm::mat4 table_translate = glm::translate(
    // //     glm::mat4( 1.0f ),
    // //     glm::vec3( 5.f, 0.f, 15.f )
    // // );

    // glm::mat4 table_model = glm::scale(  // Scale first
    //     table_translate,              // Translate second
    //     glm::vec3( 0.05f, 0.1f, 0.05f )
    // );

    table->set_position(glm::vec3( 5.f, 0.f, 15.f ));

    renderer->draw();
    compute_physics(dt);
}


void Scene::interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down) {
    //fluid->interaction(origin, direction, mouse_down);
    // cloth->interaction(origin, direction, mouse_down);
}

void Scene::key_down(int key) {}

void Scene::clear() {}

void Scene::draw_text(float x, float y, const char *string){
    RenderText(string, 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
}

void Scene::cleanup() {
    renderer->cleanup();
    delete renderer;
}