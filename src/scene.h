#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>

#include "utils/roadmap.h"
#include "renderers/phong.h"
#include "renderers/lines.h"
#include "entities/entity.h"
#include "geometry/particles.h"
#include "geometry/model.h"

#include "world/world.h"

static const glm::vec3 up(0.0f, 1.0f, 0.0f);

class Scene {
        World * world;
        Particles * particles; 
        Entity * selected_entity = nullptr;

        GLuint vao;

        Phong * renderer;
        LineRenderer * path_renderer;
        std::vector<Entity*> entities;
        std::map<std::string, Model*> loaded_models;

        Roadmap * roadmap;
        
        void init_static_uniforms();
        void draw_model(glm::mat4 matrix_model, GLuint model_vao, GLuint model_size);
        void populate_roadmap(Entity * entity);

        glm::vec3 find_target(glm::vec3 origin, glm::vec3 direction);
    public:
        Scene();
        Model * load_model(std::string path, const glm::vec3 & scale);
        orientation_state get_random_orientation(bool ground);

        std::vector<Entity*> get_nearby_entities(Entity * nearby, float radius);
        
        bool check_collisions(const orientation_state & a, const orientation_state & b, Entity * entity);
        std::vector<orientation_state> find_path(orientation_state destination, Entity * entity);

        void add_entity(Entity * entity);
        void add_renderable(Renderable * renderable, Renderable * parent = nullptr);
        void add_light(glm::vec4 pos, glm::vec3 strength, float attenuation, float ambient);

        void print_stats();
        
        void init();
        void draw(float dt);
        void update(float dt);
        void interaction(glm::vec3 origin, glm::vec3 direction);
        void key_down(int key);
        void reset();

        void cleanup();
};

namespace Globals {
	extern Scene * scene;
}

#endif
