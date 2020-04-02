#include "renderable.h"

void Renderable::update_bounding_box() {
    if (_children.size() == 0) return;

    // Goes max, min
    _model_bbox = {glm::vec3(std::numeric_limits<float>::min()), glm::vec3(std::numeric_limits<float>::max())};

    for (size_t i = 0; i< _children.size(); i++) {
        _model_bbox.max = glm::max(_model_bbox.max, _children[i]->_model_bbox.max);
        _model_bbox.min = glm::min(_model_bbox.min, _children[i]->_model_bbox.min);
    }
}

orientation_state Renderable::get_current_state() {
    orientation_state state;
    state.position = _origin;
    state.rotation = _rotation;
    return state;
}

bounding_box Renderable::get_model_bounding_box() {
    // Box max and min
    bounding_box box;

    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);
    box.max = scale_matrix * glm::vec4(_model_bbox.max, 1);
    box.min = scale_matrix * glm::vec4(_model_bbox.min, 1);

    return box;
}

OBB Renderable::generate_bounding_box() {
    return OBB(get_model_bounding_box(), get_current_state());
}

glm::quat Renderable::calculate_rotation(const glm::vec3 & direction) {
        // std::cout << "Testing: " << glm::to_string(_model_direction) << std::endl;
        // _model_direction;
        glm::quat rot1 = RotationBetweenVectors(_model_direction, direction);   

        glm::vec3 desired_up(0.f, 1.f, 0.f);
        glm::vec3 right = glm::cross(direction, desired_up);
        desired_up = glm::cross(right, direction);

        glm::vec3 newUp = rot1 * glm::vec3(0.0f, 1.0f, 0.0f);

        glm::quat rot2 = RotationBetweenVectors(newUp, desired_up);

        return rot2 * rot1;
    }

// OBB Renderable::get_bounding_box(const orientation_state & for_state) {
    
//     // glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), for_state.first);
// 	// glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);;

    
 
// 	// glm::mat4 orientation_matrix = _to_parent_matrix * translate_matrix * rotation_matrix * scale_matrix;

// 	return OBB(_model_bbox, rotation_matrix);
// }