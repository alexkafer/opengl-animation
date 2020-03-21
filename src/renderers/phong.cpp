#include "phong.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common.h"
#include "../geometry/model.h"

Phong::Phong(): _objects(), gLights() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/phong.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

    Light directionalLight;
    directionalLight.position = glm::vec4(1, 0.8, 0.6, 0); //w == 0 indications a directional light
    directionalLight.intensities = glm::vec3(0.2,0.2,0.2); //weak light
    directionalLight.ambientCoefficient = 0.06f;

    gLights.push_back(directionalLight);

    Light centerLight;
    centerLight.position = glm::vec4(1.f, 4.8f, 4.6f, 1.f); //w == 1 indications a point light
    centerLight.intensities = glm::vec3(1.f, 1.f, 1.f); //strong white light
    centerLight.ambientCoefficient = 0.1f;

    gLights.push_back(centerLight);
}

size_t Phong::add_object(Renderable * entity) {
    entity->init(shader);
    _objects.push_back(entity);
    return _objects.size() - 1;
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

    check_gl_error();
}

void Phong::draw() {
    shader.enable();
    
    glUniform3fv(shader.uniform("eye"), 1, glm::value_ptr(Globals::eye_pos));
    glUniformMatrix4fv( shader.uniform("camera"), 1, GL_FALSE, glm::value_ptr(Globals::projection * Globals::view)); // viewing transformation
	// glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, ); // projection matrix

    check_gl_error();

    for(auto t=_objects.begin(); t!=_objects.end(); ++t) {
        default_phong_uniforms();
        glm::mat4 model_translate = glm::translate(
            glm::mat4( 1.0f ),
            (*t)->get_position()
        );

        glm::mat4 model = glm::scale(  // Scale first
            model_translate,              // Translate second
            (*t)->get_scale()
        );

        glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix
        check_gl_error();
        (*t)->draw(shader);
        check_gl_error();
    }

    shader.disable();
}

void Phong::cleanup() {
    // for(auto t=_objects.begin(); t!=_objects.end(); ++t)
    //     delete t->entity;

    _objects.clear();
}