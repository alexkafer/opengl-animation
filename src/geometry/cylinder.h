#ifndef CYLINDER_H
#define CYLINDER_H

#include <glm/vec3.hpp>

// A cylinder object
class Cylinder
{
	public:
		// center of the bottom base
		glm::vec3 center;
		// center of the top base
		glm::vec3 center2;
		// radius of the bases
		float radius;
		// height of the cylinder
		float height;

		Cylinder (const glm::vec3& c, float h, float r) : center (c), height (h), radius (r), center2(c.x,c.y+h,c.z) {}

		Cylinder (float x, float y, float z, float h, float r) : 
			center (glm::vec3(x, y, z)),
			height (h), 
			radius (r), 
			center2(x,y+h,z) {}
		
		bool intersect (const glm::vec3 & origin, const glm::vec3 & direction, float& t);
		bool intersect_base (const glm::vec3 & origin, const glm::vec3 & direction, const glm::vec3& c, float& t);
		glm::vec3 normal_in (const glm::vec3& p);
};

#endif