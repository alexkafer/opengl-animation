#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "tile.h"
#include "../utils/tile_solver.h"
#include "../geometry/model.h"

class World: public Renderable
{
private:
    float x_dim, y_dim, z_dim;
    std::vector<std::vector<Tile *>> tiles;
    // TileSolver solver;
    Model * world_model;
public:
    World(float x_dim, float y_dim, float z_dim);
    ~World();

    glm::vec3 get_random_point(bool ground);

    void init(Shader & shader); 
    void draw(Shader & shader); 
    void cleanup();
};

#endif // WORLD_H