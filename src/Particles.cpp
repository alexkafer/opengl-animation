#include "Particles.hpp"

#include "common.h"

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
static const float max_lives[3] = { 3.f, 1.f, 1.5f };

Particles::Particles () {
    srand(time(0));

    glEnable(GL_PROGRAM_POINT_SIZE); 
    glEnable(GL_BLEND);
}

void Particles::print_stats() {
    std::cout << "Particles: " << _positions.size() << std::endl;
}

void Particles::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/particles.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO
    glGenBuffers(2, vbo);
}

void Particles::spawn(ParticleType type, glm::vec3 location, glm::vec3 magnitude, float radius, bool face_x) { 
    int num_particles = int(500*glm::length(magnitude));

    _positions.reserve(num_particles);
    _types.reserve(num_particles);
    _velocities.reserve(num_particles);
    _colors.reserve(num_particles);
    _life_time.reserve(num_particles);

    

    for (int i = 0; i < num_particles; i++) {
        if (face_x) {
            _positions.push_back(rand_point_on_disk_x(radius) + location);
        } else {
            _positions.push_back(rand_point_on_disk_up(radius) + location);
        }
        _types.push_back(type);
        _velocities.push_back(rand_velocity(magnitude));

        glm::vec3 color = colors[type];
        color.g += 0.5f * (double) rand() / RAND_MAX;

        _colors.push_back(colors[type]);
        _life_time.push_back(0.f);
    }
}

void Particles::update(float dt, glm::vec3 ball_center, float ball_radius){
    for(size_t i = 0, size = _positions.size(); i != size; ++i)
    {
        ParticleType type = _types[i];
        float life_percent = _life_time[i] / max_lives[type];

        _positions[i] += _velocities[i] * dt;

    
        _velocities[i].x += 20.f * (ball_center.x - _positions[i].x) * dt;
        _velocities[i].y += 0.1f;
        _velocities[i].z += 20.f * (ball_center.z - _positions[i].z) * dt;
    
        _colors[i].g = fmin(_colors[i].g + dt, 1.0f);
      
        // _colors[i].a = (1 - life_percent) * 0.5f;

        bound_particle(i, ball_center, ball_radius);

        // Update the life time and purge if needed.
        if ((_life_time[i] += dt) > max_lives[type]) {
            _positions[i] = std::move(_positions.back());
            _types[i] = std::move(_types.back());
            _velocities[i] = std::move(_velocities.back());
            _colors[i] = std::move(_colors.back());
            _life_time[i] = std::move(_life_time.back());


            _positions.pop_back();
            _types.pop_back();
            _velocities.pop_back();
            _colors.pop_back();
            _life_time.pop_back();

            size--;
            i--;
        }
    }
}

struct FurthestDistance {
        FurthestDistance(const Particles& c) : particles(c) {}
        bool operator () ( const size_t & i1, const size_t & i2 ) {
            return (glm::distance(particles._positions[i1], Globals::eye_pos) < glm::distance(particles._positions[i2], Globals::eye_pos)); 
        } 
        const Particles& particles;
};

void Particles::sort_particles() {

    std::sort(_sorted_particles.begin(), _sorted_particles.end(), FurthestDistance(*this));
}




void Particles::draw(){

    sort_particles();

    size_t num_particles = _sorted_particles.size();
    glm::vec3 positions[num_particles];
    glm::vec4 colors[num_particles];

    for (size_t i = 0; i < num_particles; i++)
    {
        positions[i] = _positions[_sorted_particles[i]];
        colors[i] = _colors[_sorted_particles[i]];
    }

    shader.enable();

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindVertexArray(vao); // Bind the globally created VAO to the current context

	glUniformMatrix4fv( shader.uniform("model"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)) ); // model transformation
    glUniformMatrix4fv( shader.uniform("view"), 1, GL_FALSE, glm::value_ptr(Globals::view) ); // viewing transformation
	glUniformMatrix4fv( shader.uniform("projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection) ); // projection matrix
    
    
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");

    glEnableVertexAttribArray(attribVertexPosition);    
    glEnableVertexAttribArray(attribVertexColor);    

    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * num_particles * sizeof(float), positions, GL_DYNAMIC_DRAW);   
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0);  

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 4 * num_particles * sizeof(float), colors, GL_DYNAMIC_DRAW);   
    glVertexAttribPointer(attribVertexColor, 4, GL_FLOAT, GL_FALSE, 0, (void*) 0);  

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    
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

    return glm::vec3(x, 0.1f, z);
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
        general_direction.x + 10.f * ((float) rand() / RAND_MAX - .5f),
        general_direction.y + 10.f * ((float) rand() / RAND_MAX - .5f),
        general_direction.z + 10.f * ((float) rand() / RAND_MAX - .5f));
}

void Particles::bound_particle(size_t particle, glm::vec3 ball_center, float ball_radius) {
    if (_positions[particle].y < 0) {
        _positions[particle].y = 0;
        _velocities[particle].x += 2.f * (float) rand() / RAND_MAX - 1.f;
        _velocities[particle].y *= -0.5; //bounce factor
        _velocities[particle].z += 2.f * (float) rand() / RAND_MAX - 1.f;
        _velocities[particle] *= 0.8; //bounce factor
        return;
    }
 
    glm::vec3 diff_vec = _positions[particle] - ball_center;

    if (abs(glm::length(diff_vec)) < ball_radius) {
        glm::vec3 normal = glm::normalize(diff_vec);
        _positions[particle] = ball_center + (ball_radius * 1.05f) * normal;

        glm::vec3 vnorm = glm::dot(normal, _velocities[particle]) * normal;
        _velocities[particle] -= vnorm;
        _velocities[particle] -= 0.5f * vnorm;
        _velocities[particle].y  = 0;
    }
}