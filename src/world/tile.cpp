#include "tile.h"

#include "../common.h"

#include "../scene.h"

static const std::map<TileType,const char*> TileModels {
    // {TileType::Sign_NoParking, "Sign_NoParking.fbx"},
    // {TileType::Sign_Stop, "Sign_Stop.fbx"},
    // {TileType::Sign_Triangle, "Sign_Triangle.fbx"},
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
    // {TileType::Streetlight_Double, "Streetlight_Double.fbx"},
    // {TileType::Streetlight_Single, "Streetlight_Single.fbx"},
    // {TileType::Streetlight_Triple, "Streetlight_Triple.fbx"},
    {TileType::Streets_all, "Streets_all.fbx"},
    // {TileType::TrafficLight_2, "TrafficLight_2.fbx"},
    // {TileType::TrafficLight, "TrafficLight.fbx"}
};


Tile::Tile(TileType type): Renderable(glm::vec3(0.05f))
{
   _type = type;
}

void Tile::init(Shader & shader) {
    auto   it  = TileModels.find(_type);
    std::string path = (it == TileModels.end()) ? "Out of range" : it->second;

    std::stringstream model_ss; model_ss << MY_MODELS_DIR << "tiles/" << path;

    _model = new Model(model_ss.str(), false);
    Globals::scene->add_renderable(_model, this);
}

void Tile::draw(Shader & shader) {}

void Tile::cleanup(){}

Tile::~Tile()
{
    cleanup();
    delete _model;
}
