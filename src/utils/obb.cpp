#include "obb.h"


OBB::OBB(const bounding_box & bbox, const orientation_state & state) {    
    center = OBB::get_center(bbox) + state.position;
    half_size = OBB::get_half_size(bbox);

    this->rotation = rotation;
    glm::mat4 rotation_matrix = glm::toMat4(state.rotation);

    axis_x = rotation_matrix * glm::vec4(global_axis[0], 1);
    axis_y = rotation_matrix * glm::vec4(global_axis[1], 1);
    axis_z = rotation_matrix * glm::vec4(global_axis[2], 1);
}

glm::vec3 OBB::get_center(const bounding_box& bbox)
{   
    return glm::vec3((bbox.min.x+bbox.max.x)/2, (bbox.min.y+bbox.max.y)/2, (bbox.min.z+bbox.max.z)/2);
}

glm::vec3 OBB::get_half_size(const bounding_box& bbox)
{   
    return glm::vec3(bbox.max.x-bbox.min.x, bbox.max.y-bbox.min.y, bbox.max.z-bbox.min.z) / 2.0f;
}

// check if there's a separating plane in between the selected axes
bool OBB::getSeparatingPlane(const glm::vec3& RPos, const glm::vec3& Plane, const OBB & box2)
{
    return (fabs(glm::dot(RPos, Plane)) > 
        (fabs(glm::dot((axis_x * half_size.x), Plane)) +
        fabs(glm::dot((axis_y * half_size.y), Plane)) +
        fabs(glm::dot((axis_z * half_size.z), Plane)) +
        fabs(glm::dot((box2.axis_x * box2.half_size.x), Plane)) + 
        fabs(glm::dot((box2.axis_y * box2.half_size.y), Plane)) +
        fabs(glm::dot((box2.axis_z * box2.half_size.z), Plane))));
}

// test for separating planes in all 15 axes
bool OBB::test_obb_obb_collision(const OBB & box2)
{
    glm::vec3 RPos = box2.center - center;

    return !(getSeparatingPlane(RPos, axis_x, box2) ||
        getSeparatingPlane(RPos, axis_y, box2) ||
        getSeparatingPlane(RPos, axis_z, box2) ||
        getSeparatingPlane(RPos, box2.axis_x, box2) ||
        getSeparatingPlane(RPos, box2.axis_y, box2) ||
        getSeparatingPlane(RPos, box2.axis_z, box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_x,box2.axis_x), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_x,box2.axis_y), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_x,box2.axis_z), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_y,box2.axis_x), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_y,box2.axis_y), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_y,box2.axis_z), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_z,box2.axis_x), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_z,box2.axis_y), box2) ||
        getSeparatingPlane(RPos, glm::cross(axis_z,box2.axis_z), box2));
}
