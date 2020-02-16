#include <glm/vec3.hpp> 
#include <glm/vec4.hpp> 
#include <glm/mat4x4.hpp>

//
//	Global state variables
//
namespace Globals {
	extern int screen_width;
	extern int screen_height;
	extern float aspect;

	extern float mouse_x;
	extern float mouse_y;
	extern bool reset_mouse;

	extern bool picking_object;
	extern int selected;
	extern bool dragging_object;

	extern float yaw;
	extern float pitch;

	extern glm::vec3 eye_pos;
	extern glm::vec3 eye_dir;
	extern glm::vec4 light;

	extern glm::mat4 view;
	extern glm::mat4 projection;
}