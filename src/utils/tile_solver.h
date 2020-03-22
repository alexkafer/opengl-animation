
#ifndef TILE_SOLVER_H
#define TILE_SOLVER_H

#include <map>
#include <array>
#include <vector>

#include "../world/tile.h"

enum TileEdge { 
    Ground_Street,
    Elevated_Street,
    Bridge_Street,
    Water,
    Land,
    None
};

static const std::map<TileType, std::array<TileEdge, 4>> EdgeMap = {
    { TileType::Undecided,                  { None, None, None, None }},
    { TileType::Street_3Way_2,              { Ground_Street, Land, Ground_Street, Ground_Street }}, 
    { TileType::Street_3Way,                { Ground_Street, Land, Ground_Street, Ground_Street }},
    { TileType::Street_4Way_2,              { Ground_Street, Ground_Street, Ground_Street, Ground_Street }},
    { TileType::Street_4Way,                { Ground_Street, Ground_Street, Ground_Street, Ground_Street }},
    { TileType::Street_Bridge_Ramp,         { Land, Ground_Street, Land, Bridge_Street }}, 
    { TileType::Street_Bridge_Underpass,    { Bridge_Street, Ground_Street, Bridge_Street, Ground_Street }},
    { TileType::Street_Bridge_Water,        { Bridge_Street, Water, Bridge_Street, Water}}, 
    { TileType::Street_Bridge_WaterRamp,    { Ground_Street, Water, Bridge_Street, Water }},
    { TileType::Street_Bridge,              { Bridge_Street, Land, Bridge_Street, Land }},
    { TileType::Street_Curve,               { Ground_Street, Ground_Street, Land, Land }},
    { TileType::Street_Deadend,             { Land, Land, Land, Ground_Street }}, 
    { TileType::Street_Elevated_Ramp,       { Elevated_Street, Land, Ground_Street, Land }},
    { TileType::Street_Elevated,            { Land, Elevated_Street, Land, Elevated_Street }},
    { TileType::Street_Empty_Water,         { Water, Water, Water, Water }},
    { TileType::Street_Empty,               { Land, Land, Land, Land }},
    { TileType::Street_Straight,            { Land, Ground_Street, Land, Ground_Street }},
};

class TileSolver {
    public:
        TileSolver(size_t dims);
        TileOrientation get_type(size_t x, size_t y);
    private:
        std::vector<std::vector<TileOrientation>> tiles;
        std::vector<std::pair<std::pair<size_t, size_t>, TileOrientation>> valid;

        void solve_next();
        bool has_undecided();
        bool test_tile(std::array<TileEdge, 4> edges, std::array<TileEdge, 4> neighors, int rotation_offset);
        bool verify_pair(TileEdge a, TileEdge b);
};

#endif // TILE_SOLVER_H