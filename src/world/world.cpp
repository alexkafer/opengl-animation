#include "world.h"

#include "../scene.h"

World::World(size_t dims) {
    tiles = std::vector<std::vector<Tile *>>(dims, std::vector<Tile*>(dims));
}

World::~World() {
    cleanup();
}

void World::init(Shader & shader) {
    float offset_x = 10.f * tiles.size() / 2.f;
    for (int i = 0; i < tiles.size(); i++) { 
        float offset_z = 10.f * tiles[i].size() / 2.f;
        for (int j = 0; j < tiles[i].size(); j++) {
            tiles[i][j] = new Tile(static_cast<TileType>(rand() % Streets_all));
            tiles[i][j]->set_position(glm::vec3(i * 10.f - offset_x, 0.f, j * 10.f - offset_z));
            Globals::scene->add_renderable(tiles[i][j], this);
        }
    } 
}
void World::draw(Shader & shader) {}
void World::cleanup(){}
