#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector> 

#include "common.h"
#include "shader.hpp"

enum ParticleType { water, fire, impact };

struct Particle {
	glm::vec3 pos;
    float age;
};


class Particles {
        mcl::Shader shader;
        GLuint vao;
        GLuint vbo[2];

        std::vector <size_t> _sorted_particles;
        std::vector <ParticleType> _types;
        std::vector <glm::vec3> _velocities;
        std::vector <glm::vec4> _colors;
        std::vector <float> _life_time;

        glm::vec3 rand_point_on_disk_up(float radius);
        glm::vec3 rand_point_on_disk_x(float radius);
        glm::vec3 rand_velocity(glm::vec3 general_direction);
        void bound_particle(size_t particle, glm::vec3 ball_center, float ball_radius);

        void sort_particles();
        friend bool Furthest(size_t i1, size_t i2);

    public:
        Particles();
        std::vector <glm::vec3> _positions;
        void print_stats();
        void init();

        void spawn(ParticleType type, glm::vec3 location, glm::vec3 magnitude, float radius, bool face_x);
        void update(float dt, glm::vec3 ball_center, float ball_radius);
        void draw();
        void cleanup();
};

#endif // PARTICLE_HPP
