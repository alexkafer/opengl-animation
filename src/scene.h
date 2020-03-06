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
        LineRenderer * path_renderer;
        std::vector<Entity*> entities;

        Roadmap * roadmap;
        
        void init_static_uniforms();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);

        bool check_collision(glm::vec3 a, glm::vec3 b);
        void update_roadmap();
    public:
        Scene();

        Roadmap * get_roadmap();
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
