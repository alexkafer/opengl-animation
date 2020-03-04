#include "phong.h"

#include <glm/gtc/type_ptr.hpp>

#include "../common.h"
#include "../geometry/model.h"

Phong::Phong(): _objects() {
    std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/phong.";
    shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );
}

size_t Phong::add_object(Renderable * entity) {
    entity->init(shader);
    _objects.push_back(entity);
    return _objects.size() - 1;
}

    ///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Phong::phong_uniforms() {
    GLint uniformLightPosition             = shader.uniform("lightPosition");
    // GLint uniformLightAmbient              = shader.uniform("lightAmbient");
    // GLint uniformLightDiffuse              = shader.uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader.uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    check_gl_error();

    // set uniform values
    float lightPosition[]  = {0.f, 5.0f, -3.f, 1.0f};
    // float lightAmbient[]  = {0.3f, 0.1f, 0.1f, 1};
    // float lightDiffuse[]  = {0.7f, 0.2f, 0.2f, 1};
    float lightSpecular[] = {1.0f, 1.0f, 1.0f, 1};
    float materialAmbient[]  = {0.4f, 0.4f, 0.4f, 1};
    float materialDiffuse[]  = {0.5f, 0.5f, 0.5f, 1};
    float materialSpecular[] = {0.4f, 0.4f, 0.4f, 1};
    float materialShininess  = 4;

    glUniform4fv(uniformLightPosition, 1, lightPosition);
    // glUniform4fv(uniformLightAmbient, 1, lightAmbient);
    // glUniform4fv(uniformLightDiffuse, 1, lightDiffuse);
    glUniform4fv(uniformLightSpecular, 1, lightSpecular);
    glUniform4fv(uniformMaterialAmbient, 1, materialAmbient);
    glUniform4fv(uniformMaterialDiffuse, 1, materialDiffuse);
    glUniform4fv(uniformMaterialSpecular, 1, materialSpecular);
    glUniform1f(uniformMaterialShininess, materialShininess);

    check_gl_error();
}

void Phong::draw() {
    shader.enable();

    phong_uniforms();

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix

    for(auto t=_objects.begin(); t!=_objects.end(); ++t) {

        glm::mat4 model_translate = glm::translate(
            glm::mat4( 1.0f ),
            (*t)->get_position()
        );

        glm::mat4 model = glm::scale(  // Scale first
            model_translate,              // Translate second
            (*t)->get_scale()
        );

        glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(model)); // model matrix

        glm::mat4 model_normal = model;
        model_normal[3] = glm::vec4(0,0,0,1);

        glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(model)); // projection matrix
	    (*t)->draw(shader);
    }

    shader.disable();
}

void Phong::cleanup() {
    // for(auto t=_objects.begin(); t!=_objects.end(); ++t)
    //     delete t->entity;

    _objects.clear();
}