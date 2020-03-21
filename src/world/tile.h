#ifndef TILE_H
#define TILE_H

#include "../geometry/model.h"
#include "../utils/shader.h"
#include "../renderers/renderable.h"

enum TileType { 
    Street_3Way_2,
    Street_3Way,
    Street_4Way_2,
    Street_4Way,
    Street_Bridge_Ramp,
    Street_Bridge_Underpass,
    Street_Bridge_Water,
    Street_Bridge_WaterRamp,
    Street_Bridge,
    Street_Curve,
    Street_Deadend,
    Street_Elevated_Ramp,
    Street_Elevated,
    Street_Empty_Water,
    Street_Empty,
    Street_Straight,
    Streets_all,
};

class Tile: public Renderable
{
private:
    /* data */
    Model * _model;
    TileType _type;
public:
    Tile(TileType type);
    ~Tile();

    void init(Shader & shader); 
    void draw(Shader & shader); 
    void cleanup();
};

#endif // TILE_H