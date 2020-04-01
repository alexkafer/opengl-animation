#ifndef OBB_H
#define OBB_H

#include <array>


#include "../common.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


const static std::array<glm::vec3, 3> global_axis = {
        glm::vec3(1.f, 0.f, 0.f),
        glm::vec3(0.f, 1.f, 0.f),
        glm::vec3(0.f, 0.f, 1.f)
};


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


struct bounding_box {
	glm::vec3 max;
	glm::vec3 min;
};

// set the relevant elements of our oriented bounding box
class OBB
{
    glm::vec3 get_center(const bounding_box& bbox);
    glm::vec3 get_half_size(const bounding_box& bbox);
    bool getSeparatingPlane(const glm::vec3& RPos, const glm::vec3& Plane, const OBB & box2);
public:
    OBB(const bounding_box & bbox, const orientation_state & rotation);

    glm::vec3 center;
	glm::vec3 axis_x;
	glm::vec3 axis_y;
	glm::vec3 axis_z;
	glm::vec3 half_size;

    glm::mat4 rotation_matrix;

    bool test_obb_obb_collision(const OBB & box2);
};

#endif