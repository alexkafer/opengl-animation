#ifndef SCENE_HPP
#define SCENE_HPP

#include "common.h"

#include "Particles.hpp"
#include "shader.hpp"
#include "geometry/Sphere.h"

class Vertex {
    public:
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoord;
};

class Scene {
        mcl::Shader shader;

        Particles particles; 
        GLuint vao;

        Sphere sphere;
        // GLuint mobile_launcher_size;
        // GLuint mobile_launcher_vbo;

        GLuint fire_hydrant_size;
        GLuint fire_hydrant_vbo;

        glm::vec3 acceleration;
        glm::vec3 position;
        glm::vec3 velocity;

        float floorPos, wallPos;

        GLuint floor_vbo[2];
        GLuint ball_vbo, ball_ibo;


        void computePhysics(float dt);
        void init_floor();
        void init_static_uniforms();

        GLuint load_model(GLuint & vbo, std::string obj_file, std::string mtl_dir);

        void draw_ball(float dt);
        void draw_floor();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);

    public:
        Scene();

        glm::vec3 get_ball_position();
        void add_ball_velocity(glm::vec3 velocity);

        void init();

        void draw(float dt);

        void cleanup();
};




#endif
