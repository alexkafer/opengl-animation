#include "entity.h"

#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "../common.h"
#include "../scene.h"

Entity::Entity(EntityType type, glm::vec3 model_direction): Renderable(model_direction) {
    _velocity = glm::vec3(0.f);
    _type = type;
}

Entity::Entity(EntityType type): Entity(type, glm::vec3(1.f, 0.f, 0.f)) {}

void Entity::update(float dt) {
    // Update the current rotation based on direction the entity is facing
    // float current_angle = atan2( _direction.x, _direction.z);
    // _rotation = glm::quat(cos(current_angle/2.f), 0.f, sin(current_angle/2.f), 0.f);  
}

void Entity::reset() {}


std::vector<orientation_state> Entity::get_current_path() {
    return {{_origin + _direction, _direction}, {_origin, _direction}};
}

float Entity::get_radius() {
    return _radius;
}

bool Entity::test_ray(glm::vec3 ray_origin, glm::vec3 ray_direction, float& intersection_distance) {
    bounding_box bbox = get_bounding_box();

    // Should be max min, so if min is bigger than max we've got nothing
    if (bbox.first.x < bbox.second.x && bbox.first.y < bbox.second.y && bbox.first.z < bbox.second.z) return false;

    // Intersection method from Real-Time Rendering and Essential Mathematics for Games
	
	float tMin = 0.0f;
	float tMax = 100000.0f;

    glm::mat4 model_matrix = get_last_model();

	glm::vec3 OBBposition_worldspace(model_matrix[3].x, model_matrix[3].y, model_matrix[3].z);

	glm::vec3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		glm::vec3 xaxis(model_matrix[0].x, model_matrix[0].y, model_matrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if ( fabs(f) > 0.001f ){ // Standard case

			float t1 = (e+bbox.second.x)/f; // Intersection with the "left" plane
			float t2 = (e+bbox.first.x)/f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1>t2){
				float w=t1;t1=t2;t2=w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if ( t2 < tMax )
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if ( t1 > tMin )
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin )
				return false;

		}else{ // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if(-e+bbox.second.x > 0.0f || -e+bbox.first.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		glm::vec3 yaxis(model_matrix[1].x, model_matrix[1].y, model_matrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+bbox.second.y)/f;
			float t2 = (e+bbox.first.y)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+bbox.second.y > 0.0f || -e+bbox.first.y < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		glm::vec3 zaxis(model_matrix[2].x, model_matrix[2].y, model_matrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if ( fabs(f) > 0.001f ){

			float t1 = (e+bbox.second.z)/f;
			float t2 = (e+bbox.first.z)/f;

			if (t1>t2){float w=t1;t1=t2;t2=w;}

			if ( t2 < tMax )
				tMax = t2;
			if ( t1 > tMin )
				tMin = t1;
			if (tMin > tMax)
				return false;

		}else{
			if(-e+bbox.second.z > 0.0f || -e+bbox.first.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;
}