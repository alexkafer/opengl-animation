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

    Light(const glm::vec4 & position_, const glm::vec3 & intensities_, float attenuation_, float ambientCoefficient)
    : position(position_), intensities(intensities_), attenuation(attenuation_), ambientCoefficient(ambientCoefficient) {}
};

class Phong
{
    std::vector<Renderable *> _objects;
    Shader shader;
    std::vector<Light> gLights;
    void default_phong_uniforms();
    void render_objects(std::vector<Renderable *> renderables, glm::mat4 parent_model);

    public:
        Phong();

        size_t add_model(const std::string &directory, const std::string &obj_file);
        void add_object(Renderable * object, Renderable * parent = nullptr);
        void add_light(glm::vec4 pos, glm::vec3 strength, float attenuation, float ambient); 
        void draw();
        void cleanup();
};
#endif 