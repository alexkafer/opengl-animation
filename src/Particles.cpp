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

static int MaxParticles = 1000000;

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
    std::cout << "Particles: " << _particles.size() << std::endl;
}

void Particles::init() {
    std::stringstream ss; ss << MY_SRC_DIR << "shaders/particles.";
    shader.init_from_files( ss.str()+"vert", ss.str()+"frag" );

    glGenVertexArrays(1, &vao); //Create a VAO

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
    };

    glGenBuffers(1, &billboard_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    glGenBuffers(1, &particles_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, particles_buffer);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * sizeof(Particle), NULL, GL_STREAM_DRAW);

    int w, h;
    int comp;

    std::stringstream texture_ss; texture_ss << MY_MODELS_DIR << "/particles/FireAtlas.png";

    unsigned char* image = stbi_load(texture_ss.str().c_str(), &w, &h, &comp, STBI_default);

    if (!image) {
        std::cerr << "Unable to load texture: " << texture_ss.str()
                << std::endl;
        exit(1);
    }
    std::cout << "Loaded texture: " << texture_ss.str() << ", w = " << w
                << ", h = " << h << ", comp = " << comp << std::endl;

    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA,
                                    GL_UNSIGNED_BYTE, image);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(image);

    std::vector<glm::vec2> UVs;
	for ( unsigned int age=0 ; age < 64 ; age++ ){
		
		float uv_x = (age%8)/8.0f;
		float uv_y = (age/8)/8.0f;

		glm::vec2 uv_up_left    = glm::vec2( uv_x           , uv_y );
		glm::vec2 uv_up_right   = glm::vec2( uv_x+1.0f/8.0f, uv_y );
		glm::vec2 uv_down_right = glm::vec2( uv_x+1.0f/8.0f, (uv_y + 1.0f/8.0f) );
		glm::vec2 uv_down_left  = glm::vec2( uv_x           , (uv_y + 1.0f/8.0f) );
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left );
		UVs.push_back(uv_up_right  );

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);
	}

    glGenBuffers(1, &age_uv_buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, age_uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
}

void Particles::spawn(ParticleType type, glm::vec3 location, glm::vec3 magnitude, float radius, bool face_x) { 

    int num_particles = int(500*glm::length(magnitude));

    _particles.reserve(num_particles);

    for (int i = 0; i < num_particles; i++) {
        glm::vec3 position;
        if (face_x) {
           position = (rand_point_on_disk_x(radius) + location);
        } else {
            position = (rand_point_on_disk_up(radius) + location);
        }

        glm::vec4 color = colors[type];
        color.g += 0.5f * (double) rand() / RAND_MAX;

        _particles.emplace_back(position, 10.f, rand_velocity(magnitude), color, type);
    }
}

void Particles::update(float dt, glm::vec3 ball_center, float ball_radius){
    for(size_t i = 0, size = _particles.size(); i != size; ++i)
    {
        float life_percent = _particles[i].life_time / max_lives[_particles[i].type];

        _particles[i].position += _particles[i].velocity * dt;

        _particles[i].velocity.x += 10.f * (ball_center.x - _particles[i].position.x) * dt;
        // _particles[i].velocity.y += 0.1f;
        _particles[i].velocity.z += 10.f * (ball_center.z - _particles[i].position.z) * dt;

         _particles[i].color.a = (1 - life_percent) * 0.5f;
       
        _particles[i].color.g = 1 - life_percent; //fmin(_particles[i].color.g + dt, 1.0f);

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

	glUniformMatrix4fv( shader.uniform("view_projection"), 1, GL_FALSE, glm::value_ptr(Globals::projection * Globals::view) ); // view projection


    glUniform3f(shader.uniform("camera_right_worldspace"), Globals::view[0][0], Globals::view[1][0], Globals::view[2][0]);
    glUniform3f(shader.uniform("camera_up_worldspace"), Globals::view[0][1], Globals::view[1][1], Globals::view[2][1]);
    
    
    GLint attribVertexBillboard  = shader.attribute("in_billboard");
    GLint attribVertexPosition  = shader.attribute("in_position");
    GLint attribVertexColor     = shader.attribute("in_color");
    GLint attribVertexTextureCoord     = shader.attribute("in_texture_coord");
    GLint attribVertexSize     = shader.attribute("in_size");

    glEnableVertexAttribArray(attribVertexBillboard);    
    glEnableVertexAttribArray(attribVertexPosition);    
    glEnableVertexAttribArray(attribVertexColor);    
    glEnableVertexAttribArray(attribVertexTextureCoord);    
    glEnableVertexAttribArray(attribVertexSize);    

    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
    glVertexAttribPointer(attribVertexBillboard, 3, GL_FLOAT, GL_FALSE, 0, (void*) 0); 

    glBindBuffer(GL_ARRAY_BUFFER, age_uv_buffer_id);
    glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, 0, (void*) 0); 

    sort(_particles.begin(), _particles.end(), CompareParticles);
    
    glBindBuffer(GL_ARRAY_BUFFER, particles_buffer);
    glBufferData(GL_ARRAY_BUFFER, MaxParticles * sizeof(Particle), NULL, GL_STREAM_DRAW); 
    glBufferSubData(GL_ARRAY_BUFFER, 0,  _particles.size() * sizeof(Particle), _particles.data());

    
    glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*) 0); 
    glVertexAttribPointer(attribVertexSize, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, size));  
    glVertexAttribPointer(attribVertexColor, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, color));  
    glVertexAttribPointer(attribVertexTextureCoord, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid*)offsetof(Particle, life_time));  

    glVertexAttribDivisor(attribVertexBillboard, 0); // particles vertices : always reuse the same 4 vertices -> 0
    glVertexAttribDivisor(attribVertexSize, 1); //  color : one per quad -> 1
    glVertexAttribDivisor(attribVertexPosition, 1); // positions : one per quad (its center) -> 1
    glVertexAttribDivisor(attribVertexColor, 1); // color : one per quad -> 1
    glVertexAttribDivisor(attribVertexTextureCoord, 1); // color : one per quad -> 1

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, _particles.size());   // # of particples

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
        general_direction.x + 5.f * ((float) rand() / RAND_MAX - .5f),
        general_direction.y + 5.f * ((float) rand() / RAND_MAX - .5f),
        general_direction.z + 5.f * ((float) rand() / RAND_MAX - .5f));
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
        particle.position = ball_center + (ball_radius * 1.05f) * normal;

        // glm::vec3 vnorm = glm::dot(normal, particle.velocity) * normal;
        // particle.velocity -= vnorm;
        // particle.velocity -= 0.5f * vnorm;
        // particle.velocity.y  = 0;
    }
}