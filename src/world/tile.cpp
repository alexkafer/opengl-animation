#include "tile.h"

Tile::Tile(TileType type)
{
    std::stringstream model_ss; model_ss << MY_MODELS_DIR << "tiles/Street_Bridge_Underpass.fbx";

   _model = new Model(model_ss.str(), false);
}

void Tile::init(Shader & shader) {
    _model->init(shader);
}
void Tile::draw(Shader & shader) {
    _model->draw(shader);
}

void Tile::cleanup(){
   _model->cleanup();
}

Tile::~Tile()
{
    cleanup();
    delete _model;
}
