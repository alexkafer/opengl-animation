#ifndef SCENE_HPP
#define SCENE_HPP

#include "common.h"

#include "Particles.hpp"

#include "geometry/model.h"
#include "geometry/Sphere.h"

static const glm::vec3 up(0.0f, 1.0f, 0.0f);

class Scene {
        mcl::Shader shader;

        Particles particles; 
        GLuint vao;
        Model * camp_fire;
        

        GLuint floor_vbo[2];

        void compute_physics(float dt);
        void check_collisions();

        void init_floor();
        void init_static_uniforms();

        void draw_floor();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);

    public:
        Scene();
        void print_stats();

        void init();

        void draw(float dt);

        void cleanup();
};




#endif
