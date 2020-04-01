#include "renderable.h"

void Renderable::update_bounding_box() {
    if (_children.size() == 0) return;

    // Goes max, min
    _bbox = {glm::vec3(std::numeric_limits<float>::min()), glm::vec3(std::numeric_limits<float>::max())};

    // glm::mat4 translate_matrix = glm::translate( glm::mat4(1.0f), _origin);
    // glm::mat4 rotation_matrix = glm::toMat4(_rotation);
    // glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), _scale);

    // glm::mat4 model = rotation_matrix * scale_matrix;

    for (size_t i = 0; i< _children.size(); i++) {
        // glm::vec4 child_max = model * glm::vec4(_children[i]->_bbox.first, 1);
        // glm::vec4 child_min = model * glm::vec4(_children[i]->_bbox.second, 1);

        // _bbox.first = glm::max(_bbox.first, glm::vec3(child_max / child_max.w));
        // _bbox.second = glm::min(_bbox.second, glm::vec3(child_min / child_min.w));

        _bbox.first = glm::max(_bbox.first, _children[i]->_bbox.first);
        _bbox.second = glm::min(_bbox.second, _children[i]->_bbox.second);
    }
}