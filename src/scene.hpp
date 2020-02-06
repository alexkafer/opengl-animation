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

        Sphere sphere;
        // GLuint mobile_launcher_size;
        // GLuint mobile_launcher_vbo;

        Model * fire_hydrant;
        Model * camp_fire;
        
        glm::vec3 acceleration;
        glm::vec3 position;
        glm::vec3 velocity;

        float floorPos, wallPos;

        GLuint floor_vbo[2];
        GLuint ball_vbo, ball_ibo;


        void compute_physics(float dt);
        void check_collisions();

        void init_floor();
        void init_static_uniforms();

        // GLuint load_model(GLuint & vbo, std::string obj_file, std::string mtl_dir);

        void draw_ball(float dt);
        void draw_floor();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);

    public:
        Scene();
        void print_stats();

        glm::vec3 get_ball_position();
        void add_ball_velocity(glm::vec3 velocity);

        void init();

        void draw(float dt);

        void cleanup();
};




#endif
