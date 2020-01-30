#ifndef SCENE_HPP
#define SCENE_HPP
#include <glm/vec3.hpp> 

#include "shader.hpp"
#include "geometry/Sphere.h"

class Scene {
        Sphere sphere;

        glm::vec3 acceleration;
        glm::vec3 position;
        glm::vec3 velocity;

        float floorPos, wallPos;

        GLuint floor_vbo[2];
        GLuint ball_vbo, ball_ibo;

        void computePhysics(float dt);
        void init_floor();

        void draw_ball(mcl::Shader * shader, float dt);
        void draw_floor(mcl::Shader * shader);

    public:

        Scene();

        glm::vec3 get_ball_position();
        void add_ball_velocity(glm::vec3 velocity);

        void init_static_uniforms(mcl::Shader * shader);
        void init_geometry(mcl::Shader * shader, GLuint & vao);

        void draw(mcl::Shader * shader, float dt);

        void cleanup();
};




#endif
