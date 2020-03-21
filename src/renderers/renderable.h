#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../common.h"
#include <glm/vec3.hpp>
#include <vector> 

#include "../utils/shader.h"

class Renderable {
protected:
    glm::vec3 _scale;
    glm::vec3 _origin;
    std::vector<Renderable *> _children;
public:
    Renderable(): _origin(0.f), _scale(1.f) {};
    Renderable(const glm::vec3 & scale): _origin(0.f), _scale(scale) {};

    virtual void init(Shader & shader) = 0; 
    virtual void draw(Shader & shader) = 0; 
    virtual void cleanup() = 0;

    virtual void set_position(glm::vec3 pos) { _origin = pos; }
    virtual glm::vec3& get_position() { return _origin; }
    virtual glm::vec3 get_scale() { return _scale; }

    std::vector<Renderable *> get_children() { return _children; }
    void add_child(Renderable * child) { _children.push_back(child);}
};

#endif // RENDERABLE_H