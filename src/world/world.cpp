#include "world.h"

#include "../scene.h"

#include "../geometry/floor.h"

// Inspired by the algorithms here:
// https://ijdykeman.github.io/ml/2017/10/12/wang-tile-procedural-generation.html

World::World(float x_dim, float y_dim, float z_dim) {
    this->x_dim = x_dim;
    this->y_dim = y_dim;
    this->z_dim = z_dim;
    // tiles = std::vector<std::vector<Tile *>>(dims, std::vector<Tile*>(dims));

    // for (int i = 0; i < tiles.size(); i++) { 
    //     for (int j = 0; j < tiles[i].size(); j++) {
    //         tiles[i][j] = new Tile(solver.get_type(i, j)); 
    //     }
    // }
    // tiles[0][0] = new Tile(std::make_pair(Street_3Way_2, 0));
    // tiles[1][0] = new Tile(std::make_pair(Street_3Way_2, 1));
    // tiles[0][1] = new Tile(std::make_pair(Street_3Way_2, 1));
    // tiles[1][1] = new Tile(std::make_pair(Street_3Way_2, 1));
}

World::~World() {
    cleanup();
}

glm::vec3 World::get_random_point(bool ground) {
    float X_HI = x_dim / 2.f;
    float X_LO = -X_HI;

    float Z_HI = z_dim / 2.f;
    float Z_LO = -Z_HI;

    return glm::vec3(
        X_LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(X_HI-X_LO))),
        ground ? 0.0f : static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(y_dim))),
        Z_LO + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(Z_HI-Z_LO))));
}

void World::init(Shader & shader) {
    // std::stringstream model_ss; model_ss << MY_MODELS_DIR << "quake/stalwart.fbx";
    // std::stringstream model_ss; model_ss << MY_MODELS_DIR << "quake/game_level/game.fbx";
    // world_model = new Model(model_ss.str(), false);
    // // world_model->set_scale(glm::vec3(2.5f));
    // world_model->set_scale(glm::vec3(0.04f));
    // Globals::scene->add_renderable(world_model, this);

    Floor * floor = new Floor();
	floor->set_scale(glm::vec3(x_dim / 2.f, 1.f, z_dim / 2.f));
	Globals::scene->add_renderable(floor, this);

    // float offset_x = (tiles.size() - 1) / 2.f;
    // for (int i = 0; i < tiles.size(); i++) { 
    //     float offset_z = (tiles[i].size() - 1) / 2.f;
    //     for (int j = 0; j < tiles[i].size(); j++) {
    //         tiles[i][j]->set_position(glm::vec3(i - offset_x, 0.f, j - offset_z));
    //         Globals::scene->add_renderable(tiles[i][j], this);
    //     }
    // } 

    // this->set_scale(glm::vec3(10.0f));
}

void World::draw(Shader & shader) {}
void World::cleanup(){}
