#ifndef TILE_H
#define TILE_H

#include "../geometry/model.h"
#include "../utils/shader.h"
#include "../renderers/renderable.h"

enum TileType { Street_4Way };

class Tile: public Renderable
{
private:
    /* data */
    Model * _model;
public:
    Tile(TileType type);
    ~Tile();

    void init(Shader & shader); 
    void draw(Shader & shader); 
    void cleanup();
};

#endif // TILE_H