#include "scene.h"

#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Scene::Scene () {
    	// Initialize the shader (which uses glew, so we need to init that first).
        // MY_SRC_DIR is a define that was set in CMakeLists.txt which gives
        // the full path to this project's src/ directory.
        std::stringstream shader_ss; shader_ss << MY_SRC_DIR << "shaders/phong.";
        shader.init_from_files( shader_ss.str()+"vert", shader_ss.str()+"frag" );

        particles = Particles();
        cloth = new Cloth(50, 50);
        // cloth = new Cloth(5, 50);
	    particles.init();
}

void Scene::print_stats() {
    particles.print_stats();
}

void Scene::compute_physics(float dt){
    cloth->update(dt);
    particles.update(dt, glm::vec3(-1.37f, 1.15f, -8.f), .6f);
}

void Scene::init_floor() {
    float vertices[] = {
        // X      Y     Z     R     G      B      U      V
        -5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
        5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 1.0f, 1.0f,
        5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 1.0f, 0.0f,
        -5.f, 0.f,  5.f, 0.376f, 0.502f, 0.22f, 0.0f, 0.0f,
        -5.f, 0.f, -5.f, 0.376f, 0.502f, 0.22f, 0.0f, 1.0f,
    };

    float normals[] = {
        0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,0.f,1.f,0.f,
    };

	glGenBuffers(2, floor_vbo);  //Create 1 buffer called vbo
	glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[0]); //Set the vbo as the active array buffer (Only one buffer can be active at a time)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); //upload vertices to vbo

    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW); //upload normals to vbo
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Scene::init()
{
    shader.enable();

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    init_floor();
    init_static_uniforms();
    cloth->init(shader);

    shader.disable();
}

///////////////////////////////////////////////////////////////////////////////
// set uniform constants
///////////////////////////////////////////////////////////////////////////////
void Scene::init_static_uniforms()
{
    GLint uniformLightPosition             = shader.uniform("lightPosition");
    // GLint uniformLightAmbient              = shader.uniform("lightAmbient");
    // GLint uniformLightDiffuse              = shader.uniform("lightDiffuse");
    GLint uniformLightSpecular             = shader.uniform("lightSpecular");
    GLint uniformMaterialAmbient           = shader.uniform("materialAmbient");
    GLint uniformMaterialDiffuse           = shader.uniform("materialDiffuse");
    GLint uniformMaterialSpecular          = shader.uniform("materialSpecular");
    GLint uniformMaterialShininess         = shader.uniform("materialShininess");
    
    // set uniform values
    float lightPosition[]  = {4.f, 3.0f, 2.f, 1.0f};
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
}

void Scene::draw_floor() {
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
	GLint attribVertexNormal    = shader.attribute("in_normal");

    // activate attribs
    glEnableVertexAttribArray(attribVertexPosition);
    glEnableVertexAttribArray(attribVertexColor);
    glEnableVertexAttribArray(attribVertexNormal);

    glm::mat4 scale_model = glm::scale(  // Scale first
        glm::mat4( 1.0f ),              // Translate second
        glm::vec3( 10.0f, 10.0f, 10.0f )
    );

    // glm::mat4 matrix_normal = glm::mat4(1.0f);
    // matrix_normal[3] = glm::vec4(0,0,0,1);

    glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(scale_model)  ); // model transformation
    glUniformMatrix4fv( shader.uniform("normal"), 1, GL_FALSE, glm::value_ptr(scale_model)); // projection matrix
		
    // set attrib arrays using glVertexAttribPointer()
    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[0]);
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), 0);
    glVertexAttribPointer(attribVertexColor, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));

    // bind vbo for floor
    glBindBuffer(GL_ARRAY_BUFFER, floor_vbo[1]);
    glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // draw a floor with VBO
    glDrawArrays(GL_TRIANGLES, 0, 6); //(Primitives, Which VBO, Number of vertices)

    glDisableVertexAttribArray(attribVertexPosition);
    glDisableVertexAttribArray(attribVertexColor);
    glDisableVertexAttribArray(attribVertexNormal);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Scene::draw(float dt) {

    shader.enable();
    glBindVertexArray(vao); // Bind the globally created VAO to the current context

    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view)  ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
		
    draw_floor();

    cloth->draw(shader);

	particles.draw();

    glBindVertexArray(0);

    compute_physics(dt);
}

Draggable* Scene::find_draggable(glm::vec3 origin, glm::vec3 direction) {
    return cloth->find_draggable(origin, direction);
}

void Scene::cleanup() {
    // Disable the shader, we're done using it
	shader.disable();

    cloth->cleanup();
    delete cloth;

	glDeleteVertexArrays(1, &vao);
}