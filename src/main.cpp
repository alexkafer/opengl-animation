// This is based on a homework assignment for CSCI5607
#ifdef __APPLE__
/* Defined before OpenGL and GLUT includes to avoid deprecation messages */
#define GL_SILENCE_DEPRECATION
#endif

// The loaders are included by glfw3 (glcorearb.h) if we are not using glew.
#ifdef USE_GLEW
#include <GL/glew.h>
#endif

#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS //ensure we are using radians

#include "common.h"
#include "scene.h"
#include "utils/GLError.h"

#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>


#include <cstring> // memcpy
#include <iostream> // memcpy


// https://github.com/syoyo/tinyobjloader

// Constants
#define WIN_WIDTH 500
#define WIN_HEIGHT 500

//
//	Global state variables
//
namespace Globals {
	int screen_width;
	int screen_height;
	float aspect;

	float mouse_x;
	float mouse_y;
	bool reset_mouse;

	bool interacting;
	bool mouse_down;

	float yaw;
	float pitch;

	glm::vec3 camera_target; // Camera is the location we will lerp to
	glm::vec3 eye_pos; // The current position of the eye
	glm::vec3 eye_dir; // The current direction 
	glm::vec4 light;

	glm::mat4 view;
	glm::mat4 projection;
}

Scene * scene; 

static const float camera_height = 1.f;
static const float camera_speed = 5.f;
static const float camera_distance = 1.f;

static void toggle_interaction(GLFWwindow* window) {
	Globals::interacting = !Globals::interacting; 

	if (Globals::interacting) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		Globals::mouse_down = false;
	} else {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		Globals::reset_mouse = true; 
		Globals::mouse_down = false;
	}
}

//
//	Callbacks
//
static void error_callback(int error, const char* description){ fprintf(stderr, "Error: %s\n", description); }

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
	// Close on escape or Q


	if( action == GLFW_PRESS || action == GLFW_REPEAT){
		switch ( key ) {
			case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GL_TRUE); break;
			case GLFW_KEY_Q: glfwSetWindowShouldClose(window, GL_TRUE); break;
			case GLFW_KEY_LEFT_SHIFT: Globals::camera_target.y -= camera_distance; break;
			case GLFW_KEY_SPACE: Globals::camera_target.y += camera_distance; break;
			case GLFW_KEY_C: scene->clear(); break;
			case GLFW_KEY_W: Globals::camera_target += camera_distance * Globals::eye_dir; break;
			case GLFW_KEY_S: Globals::camera_target -= camera_distance * Globals::eye_dir; break;
			case GLFW_KEY_A: Globals::camera_target -= 0.5f * glm::normalize(glm::cross( Globals::eye_dir, up)) * camera_distance; break;
			case GLFW_KEY_D: Globals::camera_target += 0.5f * glm::normalize(glm::cross( Globals::eye_dir, up)) * camera_distance; break;
			case GLFW_KEY_P: toggle_interaction(window);  break;
		}
		scene->key_down(key);
	}
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && Globals::interacting) {

		if (action == GLFW_PRESS) {
			std::cout << "Interacting with the scene" << std::endl;
			Globals::mouse_down = true;
		}

		if (action == GLFW_RELEASE) {
			Globals::mouse_down = false;
			Globals::reset_mouse = true;
			std::cout << "stopped picking object" << std::endl;
		}
	}
}

void calculate_eye_direction() {
	glm::vec3 direction;
	direction.x = cos(Globals::yaw)* cos(Globals::pitch);
	direction.y = sin(Globals::pitch);
	direction.z = sin(Globals::yaw)* cos(Globals::pitch);
	Globals::eye_dir = glm::normalize(direction);
}


void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	// Showing the mouse
	if (Globals::interacting) {
		// Need to convert the click into a 3D normalized device coordinates
		// and then 4d Homogeneous Clip Clip coordinates

		int width, height;
		glfwGetWindowSize(window, &width, &height);

		float x = (2.0f * xpos) / width - 1.0f;
		float y = 1.0f - (2.0f * ypos) / height;
		glm::vec4 ray_clip = glm::vec4(x, y, -1.f, 1.f);

		// Transform from the clip space to view space using the projection matrix
		glm::vec4 ray_view = glm::inverse(Globals::projection) * ray_clip;

		// We only need x and y, so z should be set to forward (in view space), and homogeneous vector
		ray_view = glm::vec4(ray_view.x, ray_view.y, -1.0, 0.0);

		glm::vec3 ray_world = glm::normalize(glm::inverse(Globals::view) * ray_view);

		// if (Globals::dragging_object) {
		// 	if (Globals::selected != -1) {
		// 		scene->drag_object(Globals::selected, ray_world);
		// 	} else {
		// 		Globals::selected = scene->find_object(Globals::eye_pos, ray_world);
		// 	}
		// }

		scene->interaction(Globals::eye_pos, ray_world, Globals::mouse_down);

		return;
	};

	// Changing camera direction
	if(Globals::reset_mouse)
    {
        Globals::mouse_x = xpos;
        Globals::mouse_y = ypos;
        Globals::reset_mouse = false;
    }

	float xoffset = xpos - Globals::mouse_x;
	float yoffset = Globals::mouse_y - ypos; // reversed since y-coordinates range from bottom to top
	
	Globals::mouse_x = xpos;
	Globals::mouse_y = ypos;

	const float sensitivity = 0.001f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Globals::yaw   += xoffset;
	Globals::pitch += yoffset;  

	if(Globals::pitch > 89.0f)
		Globals::pitch =  89.0f;
	if(Globals::pitch < -89.0f)
		Globals::pitch = -89.0f;	

	calculate_eye_direction();
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height){
	
	Globals::screen_width = width;
	Globals::screen_height = height;

	//aspect ratio needs update on resize
	Globals::aspect = Globals::screen_width/ (float)Globals::screen_height; 

	Globals::projection = glm::perspective(3.14f/4, Globals::aspect, 1.0f, 100.0f);
	
    glViewport(0,0,width,height);
}

void lookAt(glm::vec3 center)
{
	glm::vec3 direction = glm::normalize(center - Globals::eye_pos);
	Globals::pitch = asin(direction.y);
	Globals::yaw = atan2(direction.x, direction.z);

	calculate_eye_direction();
}



//
//	Main
//
int main(int argc, char *argv[]){

	// Set up window
	GLFWwindow* window;
	glfwSetErrorCallback(&error_callback);

	// Initialize the window
	if( !glfwInit() ){ return EXIT_FAILURE; }

	// Ask for OpenGL 3.2
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create the glfw window
	Globals::reset_mouse = true;
	Globals::interacting = true;
	Globals::mouse_down = false;
	window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "CSCI5611 - Alex Kafer", NULL, NULL);

	glfwGetFramebufferSize(window, &Globals::screen_width, &Globals::screen_height);
	
	if( !window ){ glfwTerminate(); return EXIT_FAILURE; }

	// Bind callbacks to the window
	glfwSetKeyCallback(window, &key_callback);
	glfwSetMouseButtonCallback(window, &mouse_button_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetFramebufferSizeCallback(window, &framebuffer_size_callback);

	// Make current
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// Initialize glew AFTER the context creation and before loading the shader.
	// Note we need to use experimental because we're using a modern version of opengl.
	#ifdef USE_GLEW
		glewExperimental = GL_TRUE;
		glewInit();
	#endif

	
	// IMPORTANT: Only call after gl context has been created
	framebuffer_size_callback(window, int(Globals::screen_width), int(Globals::screen_height)); 

	check_gl_error();

	// Initialize the scene
	scene = new Scene();
	check_gl_error();

	scene->init();
	check_gl_error();

	// Initialize OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.529f, 0.808f, .922f, 1.0f);

	// Initialize camera
	Globals::camera_target = Globals::eye_pos = glm::vec3(15.f, 2.5f, 15.f);
	
	lookAt(glm::vec3(0.f, 0.f, 0.f));

	check_gl_error();

	// Game loop
	float orbit_radius = 15.0f;
	float last_frame_time = glfwGetTime();
	float last_second_time = glfwGetTime();
	float dt = 0;
	int frame_count = 0;
	int fps = 0;

	float camera_t = 0;
	while( !glfwWindowShouldClose(window)){
	
		// Clear screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calculate time delta
		float current_frame_time = glfwGetTime();
		dt = current_frame_time - last_frame_time;
		if (dt > .1) dt = .1; //Have some max dt

		frame_count++;
		if ( current_frame_time - last_second_time >= 1.0 )
		{
			// Display the frame count here any way you want.
			std::cout << "Framerate: " << frame_count << std::endl;
			fps = frame_count;
			// scene->print_stats();
			

			frame_count = 0;
			last_second_time = current_frame_time;
		}



		last_frame_time = current_frame_time;

		Globals::camera_target.y = fmax(camera_height, Globals::camera_target.y);
		// LERP the eye towards the camera location
		glm::vec3 camera_direction = Globals::camera_target - Globals::eye_pos;
		// float camera_frac = fmin(1.0, glm::length(camera_direction) / camera_speed);

		Globals::eye_pos += camera_speed * dt * camera_direction;

		// float camX = sin(last_frame_time / 5) * orbit_radius;
		// float camZ = cos(last_frame_time / 5) * orbit_radius;

		// Calculate new view
		Globals::view = glm::lookAt(Globals::eye_pos, Globals::eye_pos + Globals::eye_dir, up);

		scene->draw(dt);
		check_gl_error();

		char text[256];
		sprintf(text,"FPS: %.2d", fps);
		scene->draw_text(0, 0, text);

		// Finalize
		glfwSwapBuffers(window);
		check_gl_error();
		
		glfwPollEvents();
		check_gl_error();

	} // end loop

	scene->cleanup();

	delete scene;

    
	return EXIT_SUCCESS;
}
