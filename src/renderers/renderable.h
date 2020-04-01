#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../common.h"
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector> 
#include <limits>       // std::numeric_limits

#include "../utils/shader.h"
#include "../utils/obb.h"

class Renderable {
private:
    glm::mat4 _model_matrix;
protected:
    glm::vec3 _origin;
    glm::vec3 _scale;
    glm::vec3 _model_direction;
    glm::vec3 _direction;
    glm::quat _rotation;
    glm::mat4 _to_parent_matrix;
    
    bounding_box _model_bbox;

    std::vector<Renderable *> _children;
public:
    Renderable(const glm::vec3 & model_direction): 
        _model_bbox({glm::vec3(std::numeric_limits<float>::min()), glm::vec3(std::numeric_limits<float>::max())}), // Goes max, min
        _direction(1.f, 0.f, 0.f),
        _model_direction(model_direction),
        _origin(0.f),
        _scale(1.f),
        _rotation(1.f, 0.f, 0.f, 0.f),
        _to_parent_matrix(1.f),
        _model_matrix(1.f) {};
    Renderable(): Renderable(glm::vec3(1.f, 0.f, 0.f)) {};

    virtual void init(Shader & shader) = 0; 
    virtual void draw(Shader & shader) = 0; 
    virtual void cleanup() = 0;

    virtual void set_parent_matrix(glm::mat4 transformation) { _to_parent_matrix = transformation; }
    virtual glm::mat4& get_to_parent_matrix() { return _to_parent_matrix; }

    const glm::mat4& get_last_model() { return _model_matrix; }

    virtual void set_position(const glm::vec3 & pos) { _origin = pos; }
    virtual glm::vec3& get_position() { return _origin; }

    glm::quat calculate_rotation(glm::vec3 direction) {
        glm::quat rot1 = RotationBetweenVectors(_model_direction, direction);   

        glm::vec3 desired_up(0.f, 1.f, 0.f);
        glm::vec3 right = glm::cross(direction, desired_up);
        desired_up = glm::cross(right, direction);

        glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);

        glm::quat rot2 = RotationBetweenVectors(newUp, desired_up);

        return rot2 * rot1;
    }

    void update_model() {
        glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), _origin);
        glm::mat4 rotation_matrix = glm::toMat4(_rotation);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);
       
        _model_matrix = _to_parent_matrix * translate_matrix * rotation_matrix * scale_matrix;
    }

    virtual void set_direction(glm::vec3 direction) { 
        _direction = direction;
        _rotation = calculate_rotation(direction);
    }

    virtual glm::vec3& get_direction() { return _direction; }
    
    virtual void set_rotation(glm::quat rotation) { 
        _rotation = rotation; 
        _direction = glm::rotate(_rotation, _model_direction);
        
    }
    virtual glm::quat& get_rotation() { return _rotation; }

    virtual glm::vec3 get_scale() { return _scale; }
    virtual void set_scale(glm::vec3 scale) {  _scale = scale; }

    virtual void update_bounding_box();

    orientation_state get_current_state();
    bounding_box get_model_bounding_box();
    OBB generate_bounding_box();

    std::vector<Renderable *> get_children() { return _children; }
    void add_child(Renderable * child) { 
        _children.push_back(child);
    }

    friend class Phong;
    friend class LineRenderer;
};

class DummyRenderable: public Renderable {
public:
    inline void init(Shader & shader) {};
    inline void draw(Shader & shader) {};
    inline void cleanup() {};
};

#endif // RENDERABLE_H