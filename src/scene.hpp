#ifndef SCENE_HPP
#define SCENE_HPP

#include "common.h"

#include "shader.hpp"
#include "geometry/Sphere.h"

class Scene {
        mcl::Shader shader;
        GLuint vao;

        Sphere sphere;

        glm::vec3 acceleration;
        glm::vec3 position;
        glm::vec3 velocity;

        float floorPos, wallPos;

        GLuint floor_vbo[2];
        GLuint ball_vbo, ball_ibo;

        void computePhysics(float dt);
        void init_floor();
        void init_static_uniforms();

        void draw_ball(float dt);
        void draw_floor();

    public:
        Scene();

        glm::vec3 get_ball_position();
        void add_ball_velocity(glm::vec3 velocity);

        void init();

        void draw(float dt);

        void cleanup();
};




#endif
