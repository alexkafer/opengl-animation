#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <vector> 

#include "common.h"
#include "shader.hpp"

class Particles {
        mcl::Shader shader;
        GLuint vao;
        GLuint vbo[2];

        std::vector <glm::vec3> _positions;
        std::vector <glm::vec3> _velocities;
        std::vector <glm::vec4> _colors;
        std::vector <float> _life_time;

        glm::vec3 rand_point_on_disk(float radius);
        glm::vec3 rand_velocity(glm::vec3 general_direction);
        void bound_particle(size_t particle);

    public:
        Particles();
        void init();

        void spawn(int num_particles);
        void update(float dt);
        void draw();
        void cleanup();
};

#endif // PARTICLE_HPP
