#include "Particles.hpp"

#include "common.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 

static glm::vec3 gravity(0.f, -9.8f, 0.f);
static glm::vec3 up(0.f, 100.f, 0.f);
static float max_life = 2.f;

Particles::Particles () {
    srand(time(0));

    glEnable(GL_PROGRAM_POINT_SIZE); 
}

void Particles::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/particles.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO
    glGenBuffers(2, vbo);
}

void Particles::spawn(int num_particles) { 

    std::cout << "Spawning " << num_particles << " particles." << std::endl;
    _positions.reserve(num_particles);
    _velocities.reserve(num_particles);
    _colors.reserve(num_particles);
    _life_time.reserve(num_particles);

    for (int i = 0; i < num_particles; i++) {
        _positions.push_back(rand_point_on_disk(2.f));
        _velocities.push_back(rand_velocity(up));
        _colors.push_back(glm::vec4(1.0f, 0.f, 0.f, 1.f));
        _life_time.push_back(0.f);
    }


    std::cout << "New amount: " << _positions.size() << std::endl;
}

void Particles::update(float dt){
    for(size_t i = 0, size = _positions.size(); i != size; ++i)
    {
        _positions[i] += _velocities[i] * dt;
        _velocities[i] += gravity * dt;

        _colors[i].g = _life_time[i] / max_life;
        _colors[i].a = max_life / (_life_time[i] + 0.01);

        bound_particle(i);

        // Update the life time and purge if needed.
        if ((_life_time[i] += dt) > max_life) {
            _positions[i] = std::move(_positions.back());
            _velocities[i] = std::move(_velocities.back());
            _colors[i] = std::move(_colors.back());
            _life_time[i] = std::move(_life_time.back());


            _positions.pop_back();
            _velocities.pop_back();
            _colors.pop_back();
            _life_time.pop_back();

            size--;
            i--;
        }
    }
}


void Particles::draw(){
    shader.enable();

	glBindVertexArray(vao); // Bind the globally created VAO to the current context

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)) ); // model transformation
    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view) ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    
    
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");

    glEnableVertexAttribArray(attribVertexPosition);    
    glEnableVertexAttribArray(attribVertexColor);    

    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * _positions.size() * sizeof(float), _positions.data(), GL_DYNAMIC_DRAW);   
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);  

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * _colors.size() * sizeof(float), _colors.data(), GL_DYNAMIC_DRAW);   
    glVertexAttribPointer(attribVertexColor, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);  

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDrawArrays(GL_POINTS, 
            0,                   // start
            _positions.size());   // # of particples

    glBindVertexArray(0);

    shader.disable();
    
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Particles::cleanup() {

}

glm::vec3 Particles::rand_point_on_disk(float radius) {
    float r = radius * (double) rand() / RAND_MAX;
    float theta  = 2 * M_PI *  (double) rand() / RAND_MAX;
    return glm::vec3(r * sin(theta), 2.0f, r * cos(theta));
}

glm::vec3 Particles::rand_velocity(glm::vec3 general_direction) {
    return glm::vec3(
        general_direction.x * 0.1 * (double) rand() / RAND_MAX,
        general_direction.y * 0.1 * (double) rand() / RAND_MAX,
        general_direction.z * 0.1 * (double) rand() / RAND_MAX);
}

void Particles::bound_particle(size_t particle) {
    if (_positions[particle].y < 0) {
        _positions[particle].y = 0;
        _velocities[particle].y *= -.4; //bounce factor
    }
}