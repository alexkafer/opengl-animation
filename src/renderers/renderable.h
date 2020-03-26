#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../common.h"
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector> 

#include "../utils/shader.h"

class Renderable {
protected:
    glm::vec3 _scale;
    glm::vec3 _origin;
    glm::quat _rotation;
    glm::mat4 _transformation;
    std::vector<Renderable *> _children;
public:
    Renderable(const glm::vec3 & scale): 
        _origin(0.f), _scale(scale), _rotation(1.f, 0.f, 0.f, 0.f), _transformation(1.f) {};
    Renderable(): Renderable(glm::vec3(1.f)) {};

    virtual void init(Shader & shader) = 0; 
    virtual void draw(Shader & shader) = 0; 
    virtual void cleanup() = 0;

    virtual void set_transformation(glm::mat4 transformation) { _transformation = transformation; }
    virtual glm::mat4& get_transformation() { return _transformation; }

    virtual void set_position(glm::vec3 pos) { _origin = pos; }
    virtual glm::vec3& get_position() { return _origin; }
    
    virtual void set_rotation(glm::quat rotation) { _rotation = rotation; }
    virtual glm::quat& get_rotation() { return _rotation; }

    virtual glm::vec3 get_scale() { return _scale; }
    virtual void set_scale(glm::vec3 scale) {  _scale = scale; }

    std::vector<Renderable *> get_children() { return _children; }
    void add_child(Renderable * child) { _children.push_back(child);}
};

class DummyRenderable: public Renderable {
public:
    inline void init(Shader & shader) {};
    inline void draw(Shader & shader) {};
    inline void cleanup() {};
};

#endif // RENDERABLE_H