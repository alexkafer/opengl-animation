#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "utils/roadmap.h"
#include "renderers/phong.h"
#include "renderers/lines.h"
#include "entities/entity.h"
#include "geometry/particles.h"

static const glm::vec3 up(0.0f, 1.0f, 0.0f);

class Scene {
        Particles * particles; 
        GLuint vao;

        Phong * renderer;
        PathRenderer * path_renderer;
        std::vector<Entity*> entities;

        Roadmap * roadmap;
        
        void init_static_uniforms();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);

        void populate_roadmap(Entity * entity);
    public:
        Scene();

        glm::vec3 find_collision(glm::vec3 origin, glm::vec3 direction);
        bool check_collisions(const glm::vec3 & a, const glm::vec3 & b, Entity * entity);
        std::vector<glm::vec3> find_path(glm::vec3 start, glm::vec3 destination, Entity * entity);

        size_t add_entity(Entity * entity);
        size_t add_renderable(Renderable * renderable);

        void print_stats();
        
        void init();
        void draw(float dt);
        void update(float dt);
        void interaction(glm::vec3 origin, glm::vec3 direction, bool mouse_down);
        void key_down(int key);
        void reset();

        void cleanup();
};

namespace Globals {
	extern Scene * scene;
}

#endif
