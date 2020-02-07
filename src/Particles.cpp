#include "Particles.hpp"

#include "common.h"
#include "stb_image.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <stdio.h> 
#include <stdlib.h> 
#include <time.h> 
#include <algorithm>    // std::sort

static glm::vec3 gravity(0.f, -9.8f, 0.f);

// static const glm::vec3 initial_directions[3] = {glm::vec3(10.f, 0.f, 0.f), glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, 0.1f, 0.f)};
// static const float initial_radius[3] = {0.075f, 0.075f, 0.5f};
static const glm::vec4 colors[3] = {glm::vec4(0.f, 0.367f, 0.645f, .7f), glm::vec4(1.f, 0.f, 0.f, 1.0f), glm::vec4(0.05f, 0.2f, 0.0f, 1.0f)};
static const float max_lives[3] = { 3.f, .75f, 1.5f };

Particles::Particles () {
    srand(time(0));

    glEnable(GL_PROGRAM_POINT_SIZE); 
    glEnable(GL_BLEND);
}

void Particles::print_stats() {
    std::cout << "Particles: " << _particles.size() << std::endl;
}

void Particles::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/particles.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO
    glGenBuffers(2, vbo);
}

void Particles::spawn(ParticleType type, glm::vec3 location, glm::vec3 magnitude, float radius, bool face_x) { 

    int num_particles = int(1000*glm::length(magnitude));

    _particles.reserve(num_particles);

    for (int i = 0; i < num_particles; i++) {
        glm::vec3 position;
        if (face_x) {
           position = (rand_point_on_disk_x(radius) + location);
        } else {
            position = (rand_point_on_disk_up(radius) + location);
        }

        _particles.emplace_back(position, .05f, rand_velocity(magnitude),  colors[type], type);
    }
}

void Particles::update(float dt, glm::vec3 ball_center, float ball_radius){
    for(size_t i = 0, size = _particles.size(); i != size; ++i)
    {
        float life_percent = _particles[i].life_time / max_lives[_particles[i].type];

        _particles[i].position += _particles[i].velocity * dt;

        _particles[i].velocity. += gravity * dt;
       
        _particles[i].color += 0.001f * glm::length(_velocities[i]);
        _particles[i].color.a = (1 - life_percent) * 0.5f;

        bound_particle(_particles[i], ball_center, ball_radius);

        if ((_particles[i].life_time += dt) > max_lives[_particles[i].type]) {
            _particles[i] = std::move(_particles.back());

            _particles.pop_back();

            size--;
            i--;
        }
    }
}

// Compares two particles and sort back to front for the rendering 
bool CompareParticles(Particle p1, Particle p2) 
{ 
    return (glm::distance( p1.position, Globals::eye_pos ) > glm::distance( p2.position, Globals::eye_pos )); 
} 

void Particles::draw(){

    shader.enable();

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texture_id);

	glBindVertexArray(vao); // Bind the globally created VAO to the current context

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

glm::vec3 Particles::rand_point_on_disk_up(float radius) {
    float r = radius * sqrt((double) rand() / RAND_MAX);
    float theta  = 2 * M_PI *  (double) rand() / RAND_MAX;
    float x = r * sin(theta);
    float z = r * cos(theta);

    return glm::vec3(x, (double) rand() / RAND_MAX, z);
}

glm::vec3 Particles::rand_point_on_disk_x(float radius) {
    float r = radius * sqrt((double) rand() / RAND_MAX);
    float theta  = 2 * M_PI *  (double) rand() / RAND_MAX;
    float y = r * sin(theta);
    float z = r * cos(theta);

    return glm::vec3((double) rand() / RAND_MAX, y, z);
}

glm::vec3 Particles::rand_velocity(glm::vec3 general_direction) {
    return glm::vec3(
        general_direction.x + ((float) rand() / RAND_MAX - .5f),
        general_direction.y + ((float) rand() / RAND_MAX - .5f),
        general_direction.z + ((float) rand() / RAND_MAX - .5f));
}

void Particles::bound_particle(Particle &particle, glm::vec3 ball_center, float ball_radius) {
    if (particle.position.y < 0) {
        particle.position.y = 0;
        particle.velocity.x += 2.f * (float) rand() / RAND_MAX - 1.f;
        particle.velocity.y *= -0.5; //bounce factor
        particle.velocity.z += 2.f * (float) rand() / RAND_MAX - 1.f;
        particle.velocity *= 0.8; //bounce factor
        return;
    }
 
    glm::vec3 diff_vec = particle.position - ball_center;

    if (abs(glm::length(diff_vec)) < ball_radius) {
        glm::vec3 normal = glm::normalize(diff_vec);
        particle.position = ball_center + (ball_radius * 1.01f) * normal;

        glm::vec3 vnorm = glm::dot(normal, particle.velocity) * normal;
        particle.velocity -= vnorm;
        particle.velocity -= 0.5f * vnorm;
        // particle.velocity.y  = 0;
    }
}