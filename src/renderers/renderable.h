#ifndef RENDERABLE_H
#define RENDERABLE_H

#include "../common.h"
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vector> 

#include "../utils/shader.h"

static glm::quat RotationBetweenVectors(glm::vec3 start, glm::vec3 dest){
	start = glm::normalize(start);
	dest = glm::normalize(dest);

	float cosTheta = glm::dot(start, dest);
	glm::vec3 rotationAxis;

	if (cosTheta < -1 + 0.001f){
		// special case when vectors in opposite directions:
		// there is no "ideal" rotation axis
		// So guess one; any will do as long as it's perpendicular to start
		rotationAxis = glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), start);
		if (glm::length2(rotationAxis) < 0.01 ) // bad luck, they were parallel, try again!
			rotationAxis = glm::cross(glm::vec3(1.0f, 0.0f, 0.0f), start);

		rotationAxis = normalize(rotationAxis);
		return glm::angleAxis(glm::radians(180.0f), rotationAxis);
	}

	rotationAxis = glm::cross(start, dest);

	float s = sqrt( (1+cosTheta)*2 );
	float invs = 1 / s;

	return glm::quat(
		s * 0.5f, 
		rotationAxis.x * invs,
		rotationAxis.y * invs,
		rotationAxis.z * invs
	);
}


class Renderable {
protected:
    glm::vec3 _scale;
    glm::vec3 _origin;
    glm::vec3 _model_direction;
    glm::vec3 _direction;
    glm::quat _rotation;
    glm::mat4 _transformation;
    std::vector<Renderable *> _children;
public:
    Renderable(const glm::vec3 & scale, const glm::vec3 & model_direction): 
        _direction(1.f, 0.f, 0.f), _model_direction(model_direction), _origin(0.f), _scale(scale), _rotation(1.f, 0.f, 0.f, 0.f), _transformation(1.f) {};
    Renderable(): Renderable(glm::vec3(1.f), glm::vec3(1.f, 0.f, 0.f)) {};

    virtual void init(Shader & shader) = 0; 
    virtual void draw(Shader & shader) = 0; 
    virtual void cleanup() = 0;

    virtual void set_transformation(glm::mat4 transformation) { _transformation = transformation; }
    virtual glm::mat4& get_transformation() { return _transformation; }

    virtual void set_position(glm::vec3 pos) { _origin = pos; }
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