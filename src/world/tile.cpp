#include "tile.h"

#include "../common.h"

#include "../scene.h"

static const std::map<TileType,const char*> TileModels {
    {TileType::Street_3Way_2, "Street_3Way_2.fbx"},
    {TileType::Street_3Way, "Street_3Way.fbx"},
    {TileType::Street_4Way_2, "Street_4Way_2.fbx"},
    {TileType::Street_4Way, "Street_4Way.fbx"},
    {TileType::Street_Bridge_Ramp, "Street_Bridge_Ramp.fbx"},
    {TileType::Street_Bridge_Underpass, "Street_Bridge_Underpass.fbx"},
    {TileType::Street_Bridge_Water, "Street_Bridge_Water.fbx"},
    {TileType::Street_Bridge_WaterRamp, "Street_Bridge_WaterRamp.fbx"},
    {TileType::Street_Bridge, "Street_Bridge.fbx"},
    {TileType::Street_Curve, "Street_Curve.fbx"},
    {TileType::Street_Deadend, "Street_Deadend.fbx"},
    {TileType::Street_Elevated_Ramp, "Street_Elevated_Ramp.fbx"},
    {TileType::Street_Elevated, "Street_Elevated.fbx"},
    {TileType::Street_Empty_Water, "Street_Empty_Water.fbx"},
    {TileType::Street_Empty, "Street_Empty.fbx"},
    {TileType::Street_Straight, "Street_Straight.fbx"},
};


Tile::Tile(TileOrientation state): Renderable(glm::vec3(0.05f))
{
   _state = state;
}

void Tile::init(Shader & shader) {
    auto   it  = TileModels.find(_state.first);
    std::string path = (it == TileModels.end()) ? "Out of range" : it->second;

    std::stringstream model_ss; model_ss << MY_MODELS_DIR << "tiles/" << path;

    _model = new Model(model_ss.str(), false);

    if (_state.second != -1) {
        _model->set_rotation(glm::angleAxis(glm::radians(-90.f * _state.second), glm::vec3(0.f, 1.f, 0.f)));
    }
    
    Globals::scene->add_renderable(_model, this);
}

void Tile::draw(Shader & shader) {}

void Tile::cleanup(){}

Tile::~Tile()
{
    cleanup();
    delete _model;
}
