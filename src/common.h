#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp> 


//
//	Global state variables
//
namespace Globals {
	extern int screenWidth;
	extern int screenHeight;
	extern float aspect;

	extern glm::vec3 eye;
	extern glm::vec4 light;

	extern glm::mat4 view;
	extern glm::mat4 projection;

	extern bool track_ball;
}