#ifndef WORLD_H
#define WORLD_H

#include <vector>
#include "tile.h"

class World: public Renderable
{
private:
    std::vector<std::vector<Tile *>> tiles;
public:
    World(size_t dims);
    ~World();

    void init(Shader & shader); 
    void draw(Shader & shader); 
    void cleanup();
};

#endif // WORLD_H