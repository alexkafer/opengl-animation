#ifndef PHONG_H
#define PHONG_H

#include <vector>
#include <glm/mat4x4.hpp>

#include "../utils/shader.h"
#include "renderable.h"

class Phong
{
    std::vector<Renderable *> _objects;
    Shader shader;

    void phong_uniforms();

    public:
        Phong();

        size_t add_model(const std::string &directory, const std::string &obj_file);
        size_t add_object(Renderable * object);
        void draw();
        void cleanup();
        void set_uniforms();
};
#endif 