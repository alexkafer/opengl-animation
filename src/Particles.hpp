#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector> 

#include "common.h"
#include "shader.hpp"

enum ParticleType { water, fire, impact };


struct Particle {
    glm::vec3 position; // w is the particle size
    float size;
    glm::vec3 velocity;
    glm::vec4 color;
    float life_time;
    ParticleType type;

    Particle(const glm::vec3 &position_, float size_, const glm::vec3& velocity_, const glm::vec4& color_, const ParticleType type_) 
        : position( position_ ), size(size_), velocity( velocity_ ), color( color_ ), life_time(0), type(type_){}
};


class Particles {
        mcl::Shader shader;
        GLuint vao;
         GLuint vbo;

        std::vector <Particle> _particles;

        glm::vec3 rand_point_on_disk_up(float radius);
        glm::vec3 rand_point_on_disk_x(float radius);
        glm::vec3 rand_velocity(glm::vec3 general_direction);
        void bound_particle(Particle &particle, glm::vec3 ball_center, float ball_radius);

        void sort_particles();
        friend bool Furthest(size_t i1, size_t i2);

    public:
        Particles();
        // std::vector <glm::vec3> _positions;
        void print_stats();
        void init();

        void spawn(ParticleType type, glm::vec3 location, glm::vec3 magnitude, float radius, bool face_x);
        void update(float dt, glm::vec3 ball_center, float ball_radius);
        void draw();
        void cleanup();
};

#endif // PARTICLE_HPP
