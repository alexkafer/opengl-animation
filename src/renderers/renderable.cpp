#include "renderable.h"

void Renderable::update_bounding_box() {
    if (_children.size() == 0) return;

    // Goes max, min
    _model_bbox = {glm::vec3(std::numeric_limits<float>::min()), glm::vec3(std::numeric_limits<float>::max())};
    // glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);

    // glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), _origin);
    // glm::mat4 rotation_matrix = glm::mat4(1.f); // glm::toMat4(_rotation);
    // glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);
    
    // _model_matrix = _to_parent_matrix * translate_matrix * rotation_matrix * scale_matrix;
    

    for (size_t i = 0; i< _children.size(); i++) {
        // glm::vec4 max = _model_matrix * glm::vec4(_children[i]->_model_bbox.max, 1);
        // glm::vec4 min = _model_matrix * glm::vec4(_children[i]->_model_bbox.min, 1);

        // _model_bbox.max = glm::max(_model_bbox.max, glm::vec3(max));
        // _model_bbox.min = glm::min(_model_bbox.min, glm::vec3(min));
        _model_bbox.max = glm::max(_model_bbox.max, _children[i]->_model_bbox.max);
        _model_bbox.min = glm::min(_model_bbox.min, _children[i]->_model_bbox.min);
    }
}

orientation_state Renderable::get_current_state() {
    return std::make_pair(_origin, _direction);
}

bounding_box Renderable::get_model_bounding_box() {

        // glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);
        
        // glm::vec3 max = scale_matrix * glm::vec4(_model_bbox.max, 1);
        // glm::vec3 min = scale_matrix * glm::vec4(_model_bbox.min, 1);
        return _model_bbox; //{max, min};
}

OBB Renderable::generate_bounding_box() {
    bounding_box box = get_model_bounding_box();

    glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);

    box.max = scale_matrix * glm::vec4(box.max, 1);
    box.min = scale_matrix * glm::vec4(box.min, 1);

    return OBB(box, get_current_state());
}

// OBB Renderable::get_bounding_box(const orientation_state & for_state) {
    
//     // glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), for_state.first);
// 	// glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);;

    
 
// 	// glm::mat4 orientation_matrix = _to_parent_matrix * translate_matrix * rotation_matrix * scale_matrix;

// 	return OBB(_model_bbox, rotation_matrix);
// }