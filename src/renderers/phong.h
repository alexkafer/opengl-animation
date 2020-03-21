#ifndef PHONG_H
#define PHONG_H

#include <vector>
#include <glm/mat4x4.hpp>

#include "../utils/shader.h"
#include "renderable.h"

struct Light {
    glm::vec4 position;
    glm::vec3 intensities;
    float attenuation; 
    float ambientCoefficient;
};

class Phong
{
    std::vector<Renderable *> _objects;
    Shader shader;
    std::vector<Light> gLights;
    void default_phong_uniforms();

    public:
        Phong();

        size_t add_model(const std::string &directory, const std::string &obj_file);
        size_t add_object(Renderable * object);
        void draw();
        void cleanup();
};
#endif 