#include "phong.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../common.h"
#include "../geometry/model.h"

Phong::Phong(): _objects(), gLights() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/phong.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

    add_light(
        glm::vec4(1.f, 3.f, 1.f, 1), //w == 1 indications a point light
        glm::vec3(0.9,0.9,0.9), //strong light
        5.0f, // Doesn't matter for direction
        0.5f // Little ambient
    );

    add_light(
        glm::vec4(1, 0.8, 0.6, 0), //w == 0 indications a directional light
        glm::vec3(0.2,0.2,0.2), //weak light
        0.0f, // Doesn't matter for direction
        0.9f // Little ambient
    );
}

void Phong::add_object(Renderable * entity, Renderable * parent) {
    entity->init(shader);

    if (parent) {
        parent->add_child(entity);
    } else {
        _objects.push_back(entity);
    }
}

void Phong::add_light(glm::vec4 pos, glm::vec3 strength, float attenuation, float ambient) {
    gLights.emplace_back(pos, strength, attenuation, ambient);
}

std::string CreateLightUniform(const char* propertyName, size_t lightIndex) {
    std::ostringstream ss;
    ss << "allLights[" << lightIndex << "]." << propertyName;
    return ss.str();
}

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Phong::default_phong_uniforms() {
    check_gl_error();
    
    glUniform1i(shader.uniform("numLights"), (int)gLights.size());

    check_gl_error();

    float materialSpecular[] = {0.f, 0.f, 0.f};
    float materialShininess = 1;

    for(size_t i = 0; i < gLights.size(); ++i){
        glUniform4fv(shader.uniform(CreateLightUniform("position", i)), 1, glm::value_ptr(gLights[i].position));
        glUniform3fv(shader.uniform(CreateLightUniform("intensities", i)), 1, glm::value_ptr(gLights[i].intensities));
        glUniform1f(shader.uniform(CreateLightUniform("attenuation", i)), gLights[i].attenuation);
        glUniform1f(shader.uniform(CreateLightUniform("ambientCoefficient", i)), gLights[i].ambientCoefficient);
    }

    glUniform3fv(shader.uniform("materialSpecularColor"), 1, materialSpecular);
    glUniform1f(shader.uniform("materialShininess"), materialShininess);

    glUniform1i(shader.uniform("has_bones"), false);
    glUniform1i(shader.uniform("has_textures"), false);

    check_gl_error();
}

void Phong::draw() {
    shader.enable();
    
    glUniform3fv(shader.uniform("eye"), 1, glm::value_ptr(Globals::eye_pos));
    glUniformMatrix4fv( shader.uniform("camera"), 1, GL_FALSE, glm::value_ptr(Globals::projection * Globals::view)); // viewing transformation
	// glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, ); // projection matrix

    check_gl_error();
    render_objects(_objects, glm::mat4(1.f));

    shader.disable();
}

void Phong::render_objects(std::vector<Renderable *> renderables, glm::mat4 parent_model) {
    if (renderables.size() == 0) return;
    for(auto t=renderables.begin(); t!=renderables.end(); ++t) {
        
        check_gl_error();
        default_phong_uniforms();

        glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), (*t)->get_position());
        glm::mat4 rotation_matrix = glm::toMat4((*t)->get_rotation());
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), (*t)->get_scale());
        glm::mat4 model = parent_model * (*t)->get_transformation() * translate_matrix * rotation_matrix * scale_matrix;
        // glm::mat4 model = parent_model * (*t)->get_transformation();

        glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix
        check_gl_error();
        (*t)->draw(shader);
        check_gl_error();

        // std::cout << "has children " << (*t)->get_children().size() << std::endl;
        render_objects((*t)->get_children(), model);
    }
}

void Phong::cleanup() {
    // for(auto t=_objects.begin(); t!=_objects.end(); ++t)
    //     delete t->entity;

    _objects.clear();
}