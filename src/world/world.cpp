#include "world.h"

#include "../scene.h"

// Inspired by the algorithms here:
// https://ijdykeman.github.io/ml/2017/10/12/wang-tile-procedural-generation.html

World::World(size_t dims): solver(dims) {
    tiles = std::vector<std::vector<Tile *>>(dims, std::vector<Tile*>(dims));

    for (int i = 0; i < tiles.size(); i++) { 
        for (int j = 0; j < tiles[i].size(); j++) {
            tiles[i][j] = new Tile(solver.get_type(i, j)); 
        }
    }
    // tiles[0][0] = new Tile(std::make_pair(Street_3Way_2, 0));
    // tiles[1][0] = new Tile(std::make_pair(Street_3Way_2, 1));
    // tiles[0][1] = new Tile(std::make_pair(Street_3Way_2, 1));
    // tiles[1][1] = new Tile(std::make_pair(Street_3Way_2, 1));

}

World::~World() {
    cleanup();
}


void World::init(Shader & shader) {
    float offset_x = 10.f * tiles.size() / 2.f;
    for (int i = 0; i < tiles.size(); i++) { 
        float offset_z = 10.f * tiles[i].size() / 2.f;
        for (int j = 0; j < tiles[i].size(); j++) {
            tiles[i][j]->set_position(glm::vec3(i * 10.f - offset_x, 0.f, j * 10.f - offset_z));
            Globals::scene->add_renderable(tiles[i][j], this);
        }
    } 
}

void World::draw(Shader & shader) {}
void World::cleanup(){}
