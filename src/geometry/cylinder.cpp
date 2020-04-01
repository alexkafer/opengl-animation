#include "cylinder.h"
#include <cmath>

#include <glm/glm.hpp>

// Find intersection with infinite cylinder with center in the origin
// to do that, translate the ray origin so that the center of the bottom base
// is at the origin, then calculate intersection with the canonical infinite cylinder
// and check if the ray intersects the lateral surface of the cylinder within our
// bases, if not check if it's intersecting the bases and if not, it's not intersecting
// our actual cylinder
bool Cylinder::intersect (const glm::vec3 & origin, const glm::vec3 & direction, float& t)
{
	// translate the ray origin
	glm::vec3 p0 (origin.x-center.x, origin.y-center.y, origin.z-center.z);

	// coefficients for the intersection equation
	// got them mathematically intersecting the line equation with the cylinder equation
	float a = direction.x*direction.x+direction.z*direction.z;
	float b = direction.x*p0.x +direction.z*p0.z;
	float c = p0.x*p0.x+p0.z*p0.z-radius*radius;

	float delta = b*b - a*c;

	//use epsilon because of computation errors between floats
	float epsilon = 0.00000001;

	// delta < 0 means no intersections
	if (delta < epsilon)
		return false;

	// nearest intersection
	t = (-b - sqrt (delta))/a;

	// t<0 means the intersection is behind the ray origin
	// which we don't want
	if (t<=epsilon)
		return false;

	
	float y = p0.y+t*direction.y;

	// check if we intersect one of the bases
	if (y > height+epsilon || y < -epsilon) {
		float dist;
		bool b1 = intersect_base (origin, direction, center2, dist);
		if(b1) t=dist;
		bool b2 = intersect_base (origin, direction, center, dist);
		if(b2 && dist>epsilon && t>=dist)
			t=dist;
		return b1 || b2;
	}

	return true;
}

// Calculate intersection with the base having center c
// We do this by calculating the intersection with the plane
// and then checking if the intersection is within the base circle
bool Cylinder::intersect_base (const glm::vec3 & origin, const glm::vec3 & direction, const glm::vec3& c, float& t)
{
	glm::vec3 normal = normal_in (c);
	glm::vec3 p0 (origin.x-center.x, origin.y-center.y, origin.z-center.z);
	float A = normal[0];
	float B = normal[1];
	float C = normal[2];
	float D = - (A*(c.x-center.x) +B*(c.y-center.y)+C*(c.z-center.z));

	if (A*direction[0]+B*direction[1]+C*direction[2]==0)
		return false;
	
	float dist = - (A*p0.x+B*p0.y+C*p0.z+D)/(A*direction[0]+B*direction[1]+C*direction[2]);

	float epsilon = 0.00000001;
	if (dist < epsilon)
		return false;

	glm::vec3 p;
	p.x = p0.x+dist*direction[0];
	p.y = p0.y+dist*direction[1];
	p.z = p0.z+dist*direction[2];
	if (p.x*p.x+p.z*p.z-radius*radius > epsilon)
		return false;

	t = dist;
	return true;
}

// Calculate the normal in a glm::vec3 on the surface
// it is a vertical vector in the bases and a vector
// having the direction of the vector from the axis to the glm::vec3
glm::vec3 Cylinder::normal_in (const glm::vec3& p)
{
	// glm::vec3 is on one of the bases
	if (p.x<center.x+radius && p.x>center.x-radius && p.z<center.z+radius && p.z>center.z-radius)
	{
		float epsilon = 0.00000001;
		if (p.y < center.y+height+epsilon && p.y>center.y+height-epsilon){
			return glm::vec3 (0,1,0);
		}
		if (p.y < center.y+epsilon && p.y>center.y-epsilon){
			return glm::vec3 (0,-1,0);
		}
	}

	// glm::vec3 is on lateral surface
 	glm::vec3 c0 (center.x, p.y, center.z);
 	glm::vec3 v = p-c0;
 	return glm::normalize(v);
}